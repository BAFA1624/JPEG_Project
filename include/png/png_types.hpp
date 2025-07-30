#pragma once // PNG_TYPES_HPP

#include "common/common.hpp"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <expected>
#include <iostream>
#include <memory>

namespace PNG
{

// enum class png_error_t : std::uint32_t { NONE, invalid_header, invalid_block
// };
//
// class bad_png_header : public std::runtime_error
//{
//     public:
//     bad_png_header() : std::runtime_error( "Invalid png header detected." )
//     {}
// };

// Chunk types underlying values are set to their hex values.
enum class PngChunkType : std::uint32_t {
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
     * - Interlace method (1 byte, value 0 "no interlace"/1 "Adam7 interlace") *
     * 13 bits total. "bit depth" is defined as: number of bits per sample or
     * palette index (not per pixel)
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
     * Lower case last letter = safe to copy,
     * even if application doesn't * understand it.*/
};
static constinit std::array<PngChunkType, 21> valid_png_chunk_t{
    // clang-format off
    PngChunkType::IHDR,
    PngChunkType::PLTE,
    PngChunkType::IDAT,
    PngChunkType::IEND,
    PngChunkType::bKGD,
    PngChunkType::cHRM,
    PngChunkType::dSIG,
    PngChunkType::eXIF,
    PngChunkType::gAMA,
    PngChunkType::hIST,
    PngChunkType::iCCP,
    PngChunkType::iTXt,
    PngChunkType::pHYs,
    PngChunkType::sBIT,
    PngChunkType::sPLT,
    PngChunkType::sRGB,
    PngChunkType::sTER,
    PngChunkType::tEXt,
    PngChunkType::tIME,
    PngChunkType::tRNS,
    PngChunkType::zTXt
    // clang-format on
};

constexpr bool
isValid( const PngChunkType png_chunk_type ) {
    return std::ranges::contains( valid_png_chunk_t, png_chunk_type );
}

enum class PngPixelFormat : std::uint32_t {
    grayscale = 0,
    truecolor = 2,
    indexed = 3,
    alpha_grayscale = 4,
    alpha_truecolor = 6
};

// Types for the IHDR chunk

using BitDepth = std::uint8_t;
static constinit std::array<BitDepth, 5> valid_bit_depths{ 1, 2, 4, 8, 16 };
constexpr bool
isValid( const BitDepth bit_depth ) {
    return std::ranges::contains( valid_bit_depths, bit_depth );
}

enum class ColourType : std::uint8_t {
    // clang-format off
    GREYSCALE         = 0,
    TRUE_COLOUR       = 2,
    INDEXED_COLOUR    = 3,
    GREYSCALE_ALPHA   = 4,
    TRUE_COLOUR_ALPHA = 6,
    INVALID           = 7
    // clang-format on
};
static constinit std::array<ColourType, 5> valid_colour_types{
    // clang-format off
    ColourType::GREYSCALE,
    ColourType::TRUE_COLOUR,
    ColourType::INDEXED_COLOUR,
    ColourType::GREYSCALE_ALPHA,
    ColourType::TRUE_COLOUR_ALPHA
    // clang-format on
};
constexpr bool
isValid( const ColourType colour_type ) {
    return std::ranges::contains( valid_colour_types, colour_type );
}
constexpr bool
isValid( const ColourType colour_type, const BitDepth bit_depth ) {
    bool is_valid{ false };
    switch ( colour_type ) {
    case ColourType::GREYSCALE: {
        is_valid = PNG::isValid( bit_depth );
    } break;
    case ColourType::INDEXED_COLOUR: {
        constexpr std::array<BitDepth, 4> valid_bit_depths{ 1, 2, 4, 8 };
        is_valid = PNG::isValid( bit_depth )
                   && std::ranges::contains( valid_bit_depths, bit_depth );
    } break;
    case ColourType::TRUE_COLOUR: [[fallthrough]];
    case ColourType::GREYSCALE_ALPHA: [[fallthrough]];
    case ColourType::TRUE_COLOUR_ALPHA: {
        constexpr std::array<BitDepth, 3> valid_bit_depths{ 8, 16 };
        is_valid = PNG::isValid( bit_depth )
                   && std::ranges::contains( valid_bit_depths, bit_depth );
    } break;
    case ColourType::INVALID: [[fallthrough]];
    default: is_valid = false;
    }

    return is_valid;
}

enum class CompressionMethod : std::uint8_t {
    COMPRESSION_METHOD_0 = 0,
    INVALID = 1
};
constexpr bool
isValid( const CompressionMethod compression_method ) {
    return compression_method == CompressionMethod::COMPRESSION_METHOD_0;
}

enum class FilterMethod : std::uint8_t { FILTER_METHOD_0 = 0, INVALID = 1 };
constexpr bool
isValid( const FilterMethod filter_method ) {
    return filter_method == FilterMethod::FILTER_METHOD_0;
}

enum class InterlaceMethod : std::uint8_t {
    // clang-format off
    NO_INTERLACE = 0,
    ADAM_7 = 1
    // clang-format on
};
constexpr bool
isValid( const InterlaceMethod interlace_method ) {
    return interlace_method == InterlaceMethod::NO_INTERLACE
           || interlace_method == InterlaceMethod::ADAM_7;
}

// ostream operators for png_types
std::ostream & operator<<( std::ostream &     out_stream,
                           const PngChunkType chunk_type );

std::ostream & operator<<( std::ostream &       out_stream,
                           const PngPixelFormat pixel_format );
} // namespace PNG
