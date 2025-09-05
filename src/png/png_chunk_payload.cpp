#include "png/png_chunk_payload.hpp"

#include <type_traits>

namespace PNG
{

namespace IHDR
{

constexpr IhdrChunkPayload::IhdrChunkPayload(
    const std::uint32_t width, const std::uint32_t height,
    const BitDepth bit_depth, const ColourType colour_type,
    const CompressionMethod compression_method,
    const FilterMethod filter_method, const InterlaceMethod interlace_method ) :
    PngChunkPayloadBase(
        sizeof( width ) + sizeof( height ) + sizeof( bit_depth )
            + sizeof( colour_type ) + sizeof( compression_method )
            + sizeof( filter_method ) + sizeof( interlace_method ),
        PngChunkType::IHDR ),
    width( width ),
    height( height ),
    bit_depth( bit_depth ),
    colour_type( colour_type ),
    compression_method( compression_method ),
    filter_method( filter_method ),
    interlace_method( interlace_method ) {}

IhdrChunkPayload::IhdrChunkPayload(
    const std::span<const std::byte> & raw_data ) :
    PngChunkPayloadBase(
        sizeof( width ) + sizeof( height ) + sizeof( bit_depth )
            + sizeof( colour_type ) + sizeof( compression_method )
            + sizeof( filter_method ) + sizeof( interlace_method ),
        PngChunkType::IHDR ) {
    assert( raw_data.size() == getSize() );

    // Parsing in data members:
    constexpr std::size_t width_offset{ 0 + sizeof( width ) };
    width =
        span_to_integer<std::uint32_t, std::endian::big, std::endian::native>(
            raw_data.subspan( 0, width_offset ) );

    constexpr std::size_t height_offset{ width_offset + sizeof( height ) };
    height =
        span_to_integer<std::uint32_t, std::endian::big, std::endian::native>(
            raw_data.subspan( width_offset, height_offset ) );

    constexpr std::size_t bit_depth_offset{ height_offset
                                            + sizeof( bit_depth ) };
    bit_depth = static_cast<BitDepth>(
        span_to_integer<BitDepth, std::endian::big, std::endian::native>(
            raw_data.subspan( height_offset, bit_depth_offset ) ) );

    constexpr std::size_t colour_type_offset{ bit_depth_offset
                                              + sizeof( colour_type ) };
    colour_type = static_cast<ColourType>(
        span_to_integer<std::underlying_type_t<ColourType>, std::endian::big,
                        std::endian::native>(
            raw_data.subspan( bit_depth_offset, colour_type_offset ) ) );

    constexpr std::size_t compression_method_offset{
        colour_type_offset + sizeof( compression_method )
    };
    compression_method = static_cast<CompressionMethod>(
        span_to_integer<std::underlying_type_t<CompressionMethod>,
                        std::endian::big, std::endian::native>(
            raw_data.subspan( colour_type_offset,
                              compression_method_offset ) ) );

    constexpr std::size_t filter_method_offset{ compression_method_offset
                                                + sizeof( filter_method ) };
    filter_method = static_cast<FilterMethod>(
        span_to_integer<std::underlying_type_t<FilterMethod>, std::endian::big,
                        std::endian::native>( raw_data.subspan(
            compression_method_offset, filter_method_offset ) ) );

    constexpr std::size_t interlace_method_offset{
        filter_method_offset + sizeof( interlace_method )
    };
    interlace_method = static_cast<InterlaceMethod>(
        span_to_integer<std::underlying_type_t<InterlaceMethod>,
                        std::endian::big, std::endian::native>(
            raw_data.subspan( filter_method_offset,
                              interlace_method_offset ) ) );
}

} // namespace IHDR

namespace PLTE
{

constexpr PlteChunkPayload::PlteChunkPayload(
    const std::vector<Palette> & palettes ) :
    PngChunkPayloadBase( sizeof( Palette )
                             * static_cast<std::uint32_t>( palettes.size() ),
                         PngChunkType::PLTE ),
    palettes( palettes ) {}
constexpr PlteChunkPayload::PlteChunkPayload(
    const std::span<const std::byte> & data ) :
    PngChunkPayloadBase( static_cast<std::uint32_t>( data.size() ),
                         PngChunkType::PLTE ),
    palettes() {}

} // namespace PLTE

} // namespace PNG
