#include "png/png_chunk_payload.hpp"

#include <type_traits>

namespace PNG
{

namespace IHDR
{

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
            raw_data.subspan( width_offset, sizeof( height ) ) );

    constexpr std::size_t bit_depth_offset{ height_offset
                                            + sizeof( bit_depth ) };
    bit_depth =
        span_to_integer<BitDepth, std::endian::big, std::endian::native>(
            raw_data.subspan( height_offset, sizeof( bit_depth ) ) );

    constexpr std::size_t colour_type_offset{ bit_depth_offset
                                              + sizeof( colour_type ) };
    colour_type =
        span_to_integer<ColourType, std::endian::big, std::endian::native>(
            raw_data.subspan( bit_depth_offset, sizeof( colour_type ) ) );

    constexpr std::size_t compression_method_offset{
        colour_type_offset + sizeof( compression_method )
    };
    compression_method = span_to_integer<CompressionMethod,
                                         std::endian::big,
                                         std::endian::native>(
        raw_data.subspan( colour_type_offset, sizeof( compression_method ) ) );

    constexpr std::size_t filter_method_offset{ compression_method_offset
                                                + sizeof( filter_method ) };
    filter_method =
        span_to_integer<FilterMethod, std::endian::big, std::endian::native>(
            raw_data.subspan( compression_method_offset,
                              sizeof( filter_method ) ) );

    interlace_method =
        span_to_integer<InterlaceMethod, std::endian::big, std::endian::native>(
            raw_data.subspan( filter_method_offset,
                              sizeof( interlace_method ) ) );
}

constexpr IhdrChunkPayload::IhdrChunkPayload(
    IhdrChunkPayload && other ) noexcept :
    PngChunkPayloadBase( other.getSize(), other.getChunkType() ),
    width( other.getWidth() ),
    height( other.getHeight() ),
    bit_depth( other.getBitDepth() ),
    colour_type( other.getColourType() ),
    compression_method( other.getCompressionMethod() ),
    filter_method( other.getFilterMethod() ),
    interlace_method( other.getInterlaceMethod() ) {
    other.setInvalid();
}

constexpr typename IhdrChunkPayload::IhdrChunkPayload &
IhdrChunkPayload::operator=( IhdrChunkPayload && other ) noexcept {
    if ( this != &other ) {
        PngChunkPayloadBase::operator=( std::move( other ) );

        width = other.width;
        height = other.height;
        bit_depth = other.bit_depth;
        colour_type = other.colour_type;
        compression_method = other.compression_method;
        filter_method = other.filter_method;
        interlace_method = other.interlace_method;

        other.setInvalid();
    }

    return *this;
}

} // namespace IHDR

namespace PLTE
{} // namespace PLTE


} // namespace PNG
