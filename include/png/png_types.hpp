#pragma once // PNG_TYPES_HPP

#include "common/common.hpp"

#include <algorithm>
// #include <bitset>
#include <cstdint>
#include <expected>
#include <iostream>
// #include <memory>
#include <map>
#include <ranges>
#include <variant>
#include <vector>

namespace PNG
{

// PngChunkType

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

constexpr auto valid_png_chunk = std::array<PngChunkType, 21>{
    PngChunkType::IHDR, PngChunkType::PLTE, PngChunkType::IDAT,
    PngChunkType::IEND, PngChunkType::bKGD, PngChunkType::cHRM,
    PngChunkType::dSIG, PngChunkType::eXIF, PngChunkType::gAMA,
    PngChunkType::hIST, PngChunkType::iCCP, PngChunkType::iTXt,
    PngChunkType::pHYs, PngChunkType::sBIT, PngChunkType::sPLT,
    PngChunkType::sRGB, PngChunkType::sTER, PngChunkType::tEXt,
    PngChunkType::tIME, PngChunkType::tRNS, PngChunkType::zTXt
};

constexpr bool
is_valid( const PngChunkType png_chunk_type ) {
    return std::ranges::contains( valid_png_chunk, png_chunk_type );
}

// ostream operators for png_types
std::ostream & operator<<( std::ostream &     out_stream,
                           const PngChunkType chunk_type );

enum class ChunkSizeType { Constant, Variable };

template <ChunkSizeType SizeType>
class PngChunkSize
{
    private:
    static constexpr ChunkSizeType m_size_type{ SizeType };
    std::size_t                    m_size;

    public:
    constexpr PngChunkSize()
        requires( m_size_type == ChunkSizeType::Variable )
        : m_size( 0 ) {}
    constexpr explicit PngChunkSize( const std::size_t size )
        requires( m_size_type == ChunkSizeType::Constant )
        : m_size( size ) {}
    constexpr ~PngChunkSize() = default;
    constexpr explicit PngChunkSize( const PngChunkSize & other ) = default;
    constexpr explicit PngChunkSize( PngChunkSize && other ) noexcept = default;
    constexpr PngChunkSize & operator=( const PngChunkSize & other )
        requires( m_size_type == other.m_size_type )
    = default;
    constexpr PngChunkSize & operator=( PngChunkSize && other ) noexcept
        requires( m_size_type == other.m_size_type )
    = default;

    constexpr void setSize( const std::size_t size ) noexcept
        requires( m_size_type == ChunkSizeType::Variable )
    {
        m_size = size;
    }

    [[nodiscard]] constexpr auto sizeType() const noexcept {
        return m_size_type;
    }
    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
};

PngChunkSize( const std::size_t ) -> PngChunkSize<ChunkSizeType::Constant>;
PngChunkSize() -> PngChunkSize<ChunkSizeType::Variable>;

// ChunkSizeTraits: This struct indicates whether a given PngChunk is expected
// to have a constant or variable size. It is used by the PngChunkSize2 class.

// Invalid PNG Chunks will be thrown away / handled gracefully eventually.
template <PngChunkType ChunkType>
    requires( is_valid( ChunkType ) )
struct ChunkSizeTraits
{
    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Variable };
};

// Specialisations for ChunkSizeType::Constant chunk types.

template <>
struct ChunkSizeTraits<PngChunkType::IHDR>
{
    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Constant };
    static constexpr std::size_t   m_size{ 13 };
};

template <>
struct ChunkSizeTraits<PngChunkType::IEND>
{
    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Constant };
    static constexpr std::size_t   m_size{ 0 };
};

template <>
struct ChunkSizeTraits<PngChunkType::cHRM>
{
    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Constant };
    static constexpr std::size_t   m_size{ 32 };
};

template <>
struct ChunkSizeTraits<PngChunkType::gAMA>
{
    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Constant };
    static constexpr std::size_t   m_size{ 4 };
};

template <>
struct ChunkSizeTraits<PngChunkType::sRGB>
{
    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Constant };
    static constexpr std::size_t   m_size{ 1 };
};

template <PngChunkType ChunkType>
class PngChunkSize2
{
    private:
    using traits = ChunkSizeTraits<ChunkType>;
    static constexpr bool is_const =
        ( traits::m_size_type == ChunkSizeType::Constant );

    static constexpr ChunkSizeType m_size_type{ ChunkSizeType::Variable };

    using storage_t = std::conditional_t<is_const, std::monostate, std::size_t>;
    storage_t m_size{};

    public:
    constexpr PngChunkSize2()
        requires( is_const )
    {};
    constexpr PngChunkSize2( const std::size_t size )
        requires( !is_const )
        : m_size( size ) {}

    [[nodiscard]] constexpr auto size() const noexcept {
        if constexpr ( is_const )
            return traits::m_size;
        else
            return m_size;
    }

    [[nodiscard]] static consteval auto sizeType() noexcept {
        return traits::m_size_type;
    }
    [[nodiscard]] static consteval auto chunkType() noexcept {
        return ChunkType;
    }

    constexpr void setSize( const std::size_t size ) noexcept
        requires( !is_const )
    {
        m_size = size;
    }
};

static_assert( PngChunkSize2<PngChunkType::PLTE>( 3 ).sizeType()
               == ChunkSizeType::Variable );

static_assert( PngChunkSize2<PngChunkType::IHDR>().size() == 13 );
static_assert( PngChunkSize2<PngChunkType::IHDR>::sizeType()
               == ChunkSizeType::Constant );
static_assert( PngChunkSize2<PngChunkType::IHDR>::chunkType()
               == PngChunkType::IHDR );

using SizeType = std::variant<PngChunkSize<ChunkSizeType::Constant>,
                              PngChunkSize<ChunkSizeType::Variable>>;

// PngPixelFormat

enum class PngPixelFormat : std::uint32_t {
    grayscale = 0,
    truecolor = 2,
    indexed = 3,
    alpha_grayscale = 4,
    alpha_truecolor = 6
};

constexpr std::array<PngPixelFormat, 5> valid_png_pixel_format{
    // clang-format off
    PngPixelFormat::grayscale,
    PngPixelFormat::truecolor,
    PngPixelFormat::indexed,
    PngPixelFormat::alpha_grayscale,
    PngPixelFormat::alpha_truecolor
    // clang-format on
};

constexpr bool
is_valid( const PngPixelFormat png_pixel_format ) {
    return std::ranges::contains( valid_png_pixel_format, png_pixel_format );
}

std::ostream & operator<<( std::ostream &       out_stream,
                           const PngPixelFormat pixel_format );


// Types for the IHDR chunk
namespace IHDR
{
// BitDepth

using BitDepth = std::uint8_t;

constexpr std::array<BitDepth, 5> valid_bit_depths{ 1, 2, 4, 8, 16 };

constexpr bool
is_valid( const BitDepth bit_depth ) {
    return std::ranges::contains( valid_bit_depths, bit_depth );
}

std::ostream & operator<<( std::ostream & out_stream, const BitDepth );

// ColourType

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

constexpr std::array<ColourType, 5> valid_colour_types{
    // clang-format off
    ColourType::GREYSCALE,
    ColourType::TRUE_COLOUR,
    ColourType::INDEXED_COLOUR,
    ColourType::GREYSCALE_ALPHA,
    ColourType::TRUE_COLOUR_ALPHA
    // clang-format on
};

constexpr bool
is_valid( const ColourType colour_type ) {
    return std::ranges::contains( valid_colour_types, colour_type );
}

constexpr bool
is_valid( const ColourType colour_type, const BitDepth bit_depth ) {
    bool is_valid{ false };
    switch ( colour_type ) {
    case ColourType::GREYSCALE: {
        is_valid = PNG::IHDR::is_valid( bit_depth );
    } break;
    case ColourType::INDEXED_COLOUR: {
        constexpr std::array<BitDepth, 4> valid_bit_depths{ 1, 2, 4, 8 };
        is_valid = PNG::IHDR::is_valid( bit_depth )
                   && std::ranges::contains( valid_bit_depths, bit_depth );
    } break;
    case ColourType::TRUE_COLOUR: [[fallthrough]];
    case ColourType::GREYSCALE_ALPHA: [[fallthrough]];
    case ColourType::TRUE_COLOUR_ALPHA: {
        constexpr std::array<BitDepth, 2> valid_bit_depths{ 8, 16 };
        is_valid = PNG::IHDR::is_valid( bit_depth )
                   && std::ranges::contains( valid_bit_depths, bit_depth );
    } break;
    case ColourType::INVALID: [[fallthrough]];
    default: is_valid = false;
    }

    return is_valid;
}

std::ostream & operator<<( std::ostream &   out_stream,
                           const ColourType colour_type );

// CompressionMethod

enum class CompressionMethod : std::uint8_t {
    COMPRESSION_METHOD_0 = 0,
    INVALID = 1
};

constexpr bool
is_valid( const CompressionMethod compression_method ) {
    return compression_method == CompressionMethod::COMPRESSION_METHOD_0;
}

// FilterMethod:

enum class FilterMethod : std::uint8_t { FILTER_METHOD_0 = 0, INVALID = 1 };

constexpr bool
is_valid( const FilterMethod filter_method ) {
    return filter_method == FilterMethod::FILTER_METHOD_0;
}

// InterlaceMethod:

enum class InterlaceMethod : std::uint8_t {
    // clang-format off
    NO_INTERLACE = 0,
    ADAM_7 = 1,
    INVALID = 2
    // clang-format on
};

constexpr bool
is_valid( const InterlaceMethod interlace_method ) {
    return interlace_method == InterlaceMethod::NO_INTERLACE
           || interlace_method == InterlaceMethod::ADAM_7;
}

} // namespace IHDR

namespace PLTE
{
struct Palette
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

std::ostream & operator<<( std::ostream & out_stream, const Palette palette );

} // namespace PLTE

namespace IDAT
{}

namespace IEND
{}

} // namespace PNG
