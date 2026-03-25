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
    bit_depth = static_cast<BitDepth>(
        span_to_integer<BitDepth, std::endian::big, std::endian::native>(
            raw_data.subspan( height_offset, sizeof( bit_depth ) ) ) );

    constexpr std::size_t colour_type_offset{ bit_depth_offset
                                              + sizeof( colour_type ) };
    colour_type = static_cast<ColourType>(
        span_to_integer<std::underlying_type_t<ColourType>, std::endian::big,
                        std::endian::native>(
            raw_data.subspan( bit_depth_offset, sizeof( colour_type ) ) ) );

    constexpr std::size_t compression_method_offset{
        colour_type_offset + sizeof( compression_method )
    };
    compression_method = static_cast<CompressionMethod>(
        span_to_integer<std::underlying_type_t<CompressionMethod>,
                        std::endian::big, std::endian::native>(
            raw_data.subspan( colour_type_offset,
                              sizeof( compression_method ) ) ) );

    constexpr std::size_t filter_method_offset{ compression_method_offset
                                                + sizeof( filter_method ) };
    filter_method = static_cast<FilterMethod>(
        span_to_integer<std::underlying_type_t<FilterMethod>, std::endian::big,
                        std::endian::native>( raw_data.subspan(
            compression_method_offset, sizeof( filter_method ) ) ) );

    interlace_method = static_cast<InterlaceMethod>(
        span_to_integer<std::underlying_type_t<InterlaceMethod>,
                        std::endian::big, std::endian::native>(
            raw_data.subspan( filter_method_offset,
                              sizeof( interlace_method ) ) ) );
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

constexpr IhdrChunkPayload &
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
{

constexpr std::vector<Palette>
bytes_to_palette( const std::span<const std::byte> & data ) {
    std::vector<Palette> result;
    result.reserve( data.size() / 3 );

    for ( const auto palette_values :
          data | std::views::chunk( sizeof( Palette ) /* = 3 */ ) ) {
        result.emplace_back(
            Palette{ std::to_integer<colour_t>( palette_values[0] ),
                     std::to_integer<colour_t>( palette_values[1] ),
                     std::to_integer<colour_t>( palette_values[2] ) } );
    }

    return result;
}

constexpr PlteChunkPayload::PlteChunkPayload(
    const std::vector<Palette> & palettes ) :
    PngChunkPayloadBase( sizeof( Palette )
                             * static_cast<std::uint32_t>( palettes.size() ),
                         PngChunkType::PLTE ) {
    r_channel = palettes | std::views::transform( []( const auto palette ) {
                    return palette.red;
                } )
                | std::ranges::to<std::vector<colour_t>>();
    g_channel = palettes | std::views::transform( []( const auto palette ) {
                    return palette.green;
                } )
                | std::ranges::to<std::vector<colour_t>>();
    b_channel = palettes | std::views::transform( []( const auto palette ) {
                    return palette.blue;
                } )
                | std::ranges::to<std::vector<colour_t>>();
}

constexpr PlteChunkPayload::PlteChunkPayload(
    const std::span<const std::byte> & data ) :
    PngChunkPayloadBase( static_cast<std::uint32_t>( data.size() ),
                         PngChunkType::PLTE ) {
    // TODO(chunk_size_type): add direct PLTE tests on main before expanding
    // this refactor further; for now keep invalid byte-count handling simple.
    if ( data.size() % 3 != 0 ) {
        setInvalid();
        return;
    }
    *this = PlteChunkPayload( bytes_to_palette( data ) );
}

constexpr PlteChunkPayload::PlteChunkPayload(
    PlteChunkPayload && other ) noexcept :
    PngChunkPayloadBase( other.getSize(), other.getChunkType() ),
    r_channel( other.rChannel() ),
    g_channel( other.gChannel() ),
    b_channel( other.bChannel() ) {
    assert( other.getChunkType() == PngChunkType::PLTE );
    assert( other.getSize() % 3 == 0 );
    other.setInvalid();
}

constexpr PlteChunkPayload &
PlteChunkPayload::operator=( PlteChunkPayload && other ) noexcept {
    assert( other.getChunkType() == PngChunkType::PLTE );
    assert( other.getSize() % 3 == 0 );

    r_channel = other.rChannel();
    g_channel = other.gChannel();
    b_channel = other.bChannel();

    other.setInvalid();
    return *this;
}

constexpr auto
PlteChunkPayload::getPalettes() const noexcept {
    return std::ranges::views::zip( r_channel, g_channel, b_channel )
           | std::views::transform( []( const auto & iter ) {
                 const auto & [r, g, b] = iter;
                 return Palette{ .red = r, .green = g, .blue = b };
             } )
           | std::ranges::to<std::vector<Palette>>();
}

} // namespace PLTE


} // namespace PNG
