#pragma once // PNG_TYPES_HPP

#include "common/common.hpp"

#include <bitset>
#include <cstdint>
#include <expected>
#include <iostream>
#include <memory>

namespace PNG
{

// Chunk types underlying values are set to their hex values.
enum class png_chunk_t : std::uint32_t {
    // WARNING:
    // INVALID chunk type is non-specification, i.e internal only
    INVALID = 0,

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
    IHDR = 0x49'48'44'52,
    /* PLTE:
     * Contains the palette, a list of colours
     * Essential for colour type 3 (indexed colour).
     * Optional for colour types 2 & 6 (truecolour & truecolour with alpha) *
     * Must not appear for colour types 0 & 4 (grayscale & grayscale with alpha)
     * * */
    PLTE = 0x50'4c'54'45,
    /* IDAT:
     * Contains image. May be split between multiple IDAT chunks.
     * Splitting increases filesize but allows PNG to be generated in a
     * streaming manner. IDAT chunk contains actual image data, the output of
     * the compression algorithm.
     * */
    IDAT = 0x49'44'41'54,
    /* IEND:
     * Marks end of image
     * */
    IEND = 0x49'45'4e'44,

    // ANCILLARY CHUNKS:
    bKGD = 0x62'4b'47'44, // def. background colour
    cHRM =
        0x63'48'52'4d, // chromaticity coords of display primaries & white point
    dSIG = 0x64'53'49'47, // Storing digital signatures
    eXIF = 0x65'58'49'46, // Stores Exif metadata
    // Specifies gamma. Contains 4 bytes, value = gamma * 100000
    // e.g 1/3.4 -> 29411.7647... -> 29412 for storage
    gAMA = 0x67'41'4d'41,
    hIST = 0x68'49'53'54, // Stores histogram, or tot. amount of each colour in
                          // image
    iCCP = 0x69'43'43'50, // ICC colour profile
    iTXt = 0x69'54'58'74, // Contains keyword & UTF8 text, see
    // https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_format
    pHYs =
        0x70'48'59'73, // Intended pixel sz. px/unit x-axis (4 bytes), px/unit
                       // y-axis (4
                       // bytes), unit specifier (1 byte)
    sBIT = 0x73'42'49'54, // (Significant bits) shows colour-accuracy of source
                          // data (1-13
                          // bytes)
    sPLT = 0x73'50'4c'54, // Suggest palette if fulle colour range unavailable
    sRGB = 0x73'52'47'42, // Indicates sRGB colour space is used. Contains 1
                          // byte, 4 values
                          // (0/1/2/3) are defined for rendering intent
    sTER =
        0x73'54'45'52, // Stereo-image indicator chunk for stereoscopic images
    /* tEXt:
     * Stores text.
     * One key-value pair per chunk.
     * The "key" must be between 1-79 chars long.
     * The separator is a null character.
     * The "value" can be between 0-(MAX_CHUNK_SZ - len(key) - len(separator))
     * Neither "key" or "value" can contain null chars.
     * Leading or trailing spaces also disallowed.
     * */
    tEXt = 0x74'45'58'74,
    tIME = 0x74'49'4d'45, // Stores time of last change.
    /* tRNS:
     * Contains transparency information.
     * For indexed images, stores alhpha channel vals for 1 or more palette
     * entries. For truecolour & grayscale images, stores single px val that is
     * to be regarded as transparent.
     * */
    tRNS = 0x74'52'4e'53,
    zTXt = 0x7a'54'58'74 // Contains compressed text (& compression method
                         // marker). Same
                         // limits
                         // as tEXt

    /* Lower case first letter = non-critical
     * Lower case last letter = safe to copy, even if application doesn't
     * understand it.*/
};

enum class png_pixel_format_t : std::uint32_t {
    grayscale,
    truecolor = 2,
    indexed = 3,
    alpha_grayscale = 4,
    alpha_truecolor = 6
};

enum class png_error_t : std::uint32_t { NONE, invalid_header, invalid_block };

class bad_png_header : public std::runtime_error
{
    public:
    bad_png_header() : std::runtime_error( "Invalid png header detected." ) {}
};

std::ostream & operator<<( std::ostream &    out_stream,
                           const png_chunk_t chunk_type );

std::ostream & operator<<( std::ostream &           out_stream,
                           const png_pixel_format_t pixel_format );

struct PNGChunk
{
    std::uint32_t           data_size; // Length of block
    png_chunk_t             type;      // Type of block
    std::unique_ptr<char *> block_ptr; // Pointer to start of block in memory
    std::bitset<32>         crc; // Block cyclic-redundancy-check for block

    constexpr PNGChunk() noexcept :
        data_size( 0 ),
        type( png_chunk_t::INVALID ),
        block_ptr( nullptr ),
        crc( 0 ) {}
    PNGChunk( const std::uint32_t _len, const png_chunk_t _type,
              std::unique_ptr<char *> _source_ptr,
              const std::bitset<32> & _crc ) noexcept;
    PNGChunk( const std::uint32_t _len, const png_chunk_t _type,
              char * const _source_ptr, const std::bitset<32> & _crc ) noexcept;

    NOCOPY( PNGChunk );

    PNGChunk( PNGChunk && rhs ) noexcept;
    PNGChunk & operator=( PNGChunk && rhs ) noexcept;

    PNGChunk deep_copy() const;

    [[nodiscard]] static constexpr bool
    is_valid( const png_chunk_t type ) noexcept {
        switch ( type ) {
        case png_chunk_t::IHDR: [[fallthrough]];
        case png_chunk_t::PLTE: [[fallthrough]];
        case png_chunk_t::IDAT: [[fallthrough]];
        case png_chunk_t::IEND: [[fallthrough]];
        case png_chunk_t::bKGD: [[fallthrough]];
        case png_chunk_t::cHRM: [[fallthrough]];
        case png_chunk_t::dSIG: [[fallthrough]];
        case png_chunk_t::eXIF: [[fallthrough]];
        case png_chunk_t::gAMA: [[fallthrough]];
        case png_chunk_t::hIST: [[fallthrough]];
        case png_chunk_t::iCCP: [[fallthrough]];
        case png_chunk_t::iTXt: [[fallthrough]];
        case png_chunk_t::pHYs: [[fallthrough]];
        case png_chunk_t::sBIT: [[fallthrough]];
        case png_chunk_t::sPLT: [[fallthrough]];
        case png_chunk_t::sRGB: [[fallthrough]];
        case png_chunk_t::sTER: [[fallthrough]];
        case png_chunk_t::tEXt: [[fallthrough]];
        case png_chunk_t::tIME: [[fallthrough]];
        case png_chunk_t::tRNS: [[fallthrough]];
        case png_chunk_t::zTXt: return ( true );
        case png_chunk_t::INVALID: [[fallthrough]];
        // Enums allow any valid representation of the
        // underlying type, not just the defined values.
        default: return false;
        }
    }
};

std::ostream & operator<<( std::ostream & out_stream, const PNGChunk & chunk );

} // namespace PNG
