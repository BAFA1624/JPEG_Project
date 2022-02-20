#ifndef CST_PNG_HPP_
#define CST_PNG_HPP_

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <cstring>
#include <map>

#include "CRC.hpp"

enum class _PNG_CHUNK_TYPE : uint32_t{

    INVALID,

	// CRITICAL CHUNKS:

	/* IHDR:
	 * Contains in the following order:
	 * - Image width (4 bytes).
	 * - Image height (4 bytes).
	 * - Bit depth (1 byte, value 1/2/3/8/16)
	 * - Color type (1 byte, value 0/2/3/4/6)
	 * - Compression method (1 byte, value 0)
	 * - Filter method (1 byte, value 0)
	 * - Interlace method (1 byte, value 0 "no interlace"/1 "Adam7 interlace")
	 * 13 bits total.
	 * "bit depth" is defined as:
	 * number of bits per sample or palette index (not per pixel)
	 * */
	IHDR,
	/* PLTE:
	 * Contains the palette, a list of colours
	 * Essential for colour type 3 (indexed colour).
	 * Optional for colour types 2 & 6 (truecolour & truecolour with alpha)
	 * Must not appear for colour types 0 & 4 (grayscale & grayscale with alpha)
	 * */
	PLTE,
	/* IDAT:
	 * Contains image. May be split between multiple IDAT chunks.
	 * Splitting increases filesize but allows PNG to be generated in a streaming manner.
	 * IDAT chunk contains actual image data, the output of the compression algorithm.
	 * */
	IDAT,
	/* IEND:
	 * Marks end of image
	 * */
	IEND,

	// ANCILLARY CHUNKS:

	bKGD, // def. background colour
	cHRM, // chromaticity coords of display primaries & white point
	dSIG, // Storing digital signatures
	eXIF, // Stores Exif metadata
	// Specifies gamma. Contains 4 bytes, value = gamma * 100000
	// e.g 1/3.4 -> 29411.7647... -> 29412 for storage
	gAMA,
	hIST, // Stores histogram, or tot. amount of each colour in image
	iCCP, // ICC colour profile
	iTXt, // Contains keyword & UTF8 text, see https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_format
	pHYs, // Intended pixel sz. px/unit x-axis (4 bytes), px/unit y-axis (4 bytes), unit specifier (1 byte)
	sBIT, // (Significant bits) shows colour-accuracy of source data (1-13 bytes)
	sPLT, // Suggest palette if fulle colour range unavailable
	sRGB, // Indicates sRGB colour space is used. Contains 1 byte, 4 values (0/1/2/3) are defined for rendering intent
	sTER, // Stereo-image indicator chunk for stereoscopic images
	/* tEXt:
	 * Stores text.
	 * One key-value pair per chunk.
	 * The "key" must be between 1-79 chars long.
	 * The separator is a null character.
	 * The "value" can be between 0-(MAX_CHUNK_SZ - len(key) - len(separator))
	 * Neither "key" or "value" can contain null chars.
	 * Leading or trailing spaces also disallowed.
	 * */
	tEXt,
	tIME, // Stores time of last change.
	/* tRNS:
	 * Contains transparency information.
	 * For indexed images, stores alhpha channel vals for 1 or more palette entries.
	 * For truecolour & grayscale images, stores single px val that is to be regarded as transparent.
	 * */
	tRNS,
	zTXt // Contains compressed text (& compression method marker). Same limits as tEXt

	/* Lower case first letter = non-critical
	 * Lower case last letter = safe to copy, even if application doesn't understand it.*/
};
using ChunkType = _PNG_CHUNK_TYPE;

typedef struct _PNG_CHUNK {
    uint32_t _len;
    ChunkType _type;
    char* _data;
    uint32_t _CRC;

    _PNG_CHUNK(uint32_t len, ChunkType type, char* data, uint32_t CRC) : _len(len), _type(type), _data(data), _CRC(CRC) {}
    _PNG_CHUNK() : _len(0), _type(ChunkType::IHDR), _data(nullptr), _CRC(0) {}
} PNG_CHUNK;

enum class _PNG_PIXEL_FORMAT {
    grayscale,
    truecolor = 2,
    indexed = 3,
    a_grayscale = 4,
    a_truecolor = 6
};
using PX_FORMAT = _PNG_PIXEL_FORMAT;

typedef struct _PNG_DATA {
	char header[8];
	std::vector<PNG_CHUNK> _chunk_data;
} PNG;


bool ParsePNG(const std::filesystem::directory_entry& _file, PNG& _png) noexcept;
bool InitChunk(const unsigned char* _start, PNG_CHUNK* _chunk) noexcept;
bool DeleteChunk(PNG_CHUNK* _chunk) noexcept;
ChunkType GetChunkType(const unsigned char* _chars) noexcept;

#endif // CST_PNG_HPP_