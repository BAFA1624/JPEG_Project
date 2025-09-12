#include "png/png_chunk_payload.hpp"

#include <type_traits>

namespace PNG
{

namespace IHDR
{

IhdrChunkPayload::IhdrChunkPayload(
    const std::span<const std::byte> & raw_data ) noexcept :
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

IhdrChunkPayload::IhdrChunkPayload( const IhdrChunkPayload & other ) noexcept :
    PngChunkPayloadBase(
        sizeof( width ) + sizeof( height ) + sizeof( bit_depth )
            + sizeof( colour_type ) + sizeof( compression_method )
            + sizeof( filter_method ) + sizeof( interlace_method ),
        PngChunkType::IHDR ),
    width( other.getWidth() ),
    height( other.getHeight() ),
    bit_depth( other.getBitDepth() ),
    colour_type( other.getColourType() ),
    compression_method( other.getCompressionMethod() ),
    filter_method( other.getFilterMethod() ),
    interlace_method( other.getInterlaceMethod() ) {}

IhdrChunkPayload &
IhdrChunkPayload::operator=( const IhdrChunkPayload & other ) noexcept {
    width = other.getWidth();
    height = other.getHeight();
    bit_depth = other.getBitDepth();
    colour_type = other.getColourType();
    compression_method = other.getCompressionMethod();
    filter_method = other.getFilterMethod();
    interlace_method = other.getInterlaceMethod();

    return *this;
}

IhdrChunkPayload::IhdrChunkPayload( IhdrChunkPayload && other ) noexcept :
    PngChunkPayloadBase(
        sizeof( width ) + sizeof( height ) + sizeof( bit_depth )
            + sizeof( colour_type ) + sizeof( compression_method )
            + sizeof( filter_method ) + sizeof( interlace_method ),
        PngChunkType::IHDR ),
    width( other.getWidth() ),
    height( other.getHeight() ),
    bit_depth( other.getBitDepth() ),
    colour_type( other.getColourType() ),
    compression_method( other.getCompressionMethod() ),
    filter_method( other.getFilterMethod() ),
    interlace_method( other.getInterlaceMethod() ) {
    other.setInvalid();
}

} // namespace IHDR

namespace PLTE
{} // namespace PLTE


} // namespace PNG
