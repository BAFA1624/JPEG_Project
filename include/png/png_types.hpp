#pragma once // PNG_TYPES_HPP

#include <bitset>
#include <cstdint>

enum class png_chunk_t : std::uint32_t {
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
     * Splitting increases filesize but allows PNG to be generated in a
     * streaming manner. IDAT chunk contains actual image data, the output of
     * the compression algorithm.
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
    iTXt, // Contains keyword & UTF8 text, see
          // https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_format
    pHYs, // Intended pixel sz. px/unit x-axis (4 bytes), px/unit y-axis (4
          // bytes), unit specifier (1 byte)
    sBIT, // (Significant bits) shows colour-accuracy of source data (1-13
          // bytes)
    sPLT, // Suggest palette if fulle colour range unavailable
    sRGB, // Indicates sRGB colour space is used. Contains 1 byte, 4 values
          // (0/1/2/3) are defined for rendering intent
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
     * For indexed images, stores alhpha channel vals for 1 or more palette
     * entries. For truecolour & grayscale images, stores single px val that is
     * to be regarded as transparent.
     * */
    tRNS,
    zTXt // Contains compressed text (& compression method marker). Same limits
         // as tEXt

    /* Lower case first letter = non-critical
     * Lower case last letter = safe to copy, even if application doesn't
     * understand it.*/
};

enum class png_pixel_format_t {
    grayscale,
    truecolor = 2,
    indexed = 3,
    a_grayscale = 4,
    a_truecolor = 6
};

struct PNGChunk
{
    std::uint32_t   len;
    png_chunk_t     type;
    char *          data;
    std::bitset<32> crc;

    PNGChunk( std::uint32_t _len, png_chunk_t _type, char * _data,
              std::uint32_t _crc ) :
        len( _len ), type( _type ), data( _data ), crc( _crc ) {}
    PNGChunk() :
        len( 0 ), type( png_chunk_t::INVALID ), data( nullptr ), crc( 0 ) {}
};

// PNG_TYPES_HPP
