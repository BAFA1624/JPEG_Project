#include "png/png_chunk_payload.hpp"

#include <type_traits>

namespace PNG
{

namespace IHDR
{

IhdrChunkPayload::IhdrChunkPayload(
    const std::span<const std::byte> & raw_data ) :
    PngChunkPayloadBase() {
    assert( raw_data.size() == size() );

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

} // namespace IHDR

namespace PLTE
{

std::vector<Palette>
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

PlteChunkPayload::PlteChunkPayload(
    const std::vector<Palette> & palettes ) :
    PngChunkPayloadBase( sizeof( Palette )
                         * static_cast<std::uint32_t>( palettes.size() ) ) {
    r_channel = palettes | std::views::transform( []( const auto & palette ) {
                    return palette.red;
                } )
                | std::ranges::to<std::vector<colour_t>>();
    g_channel = palettes | std::views::transform( []( const auto & palette ) {
                    return palette.green;
                } )
                | std::ranges::to<std::vector<colour_t>>();
    b_channel = palettes | std::views::transform( []( const auto & palette ) {
                    return palette.blue;
                } )
                | std::ranges::to<std::vector<colour_t>>();
}

PlteChunkPayload::PlteChunkPayload(
    const std::span<const std::byte> & data ) :
    PngChunkPayloadBase( static_cast<std::uint32_t>( data.size() ) ) {
    if ( data.size() % 3 != 0 ) {
        setInvalid();
        return;
    }

    *this = PlteChunkPayload( bytes_to_palette( data ) );
}

PlteChunkPayload::PlteChunkPayload(
    PlteChunkPayload && other ) noexcept :
    PngChunkPayloadBase( other.size() ),
    r_channel( other.rChannel() ),
    g_channel( other.gChannel() ),
    b_channel( other.bChannel() ) {
    assert( other.size() % 3 == 0 );
    other.setInvalid();
}

PlteChunkPayload &
PlteChunkPayload::operator=( PlteChunkPayload && other ) noexcept {
    if ( this != &other ) {
        PngChunkPayloadBase::operator=( std::move( other ) );
        r_channel = other.rChannel();
        g_channel = other.gChannel();
        b_channel = other.bChannel();
        other.setInvalid();
    }

    return *this;
}

std::vector<Palette>
PlteChunkPayload::getPalettes() const noexcept {
    return std::ranges::views::zip( r_channel, g_channel, b_channel )
           | std::views::transform( []( const auto & channels ) {
                 const auto & [r, g, b] = channels;
                 return Palette{ .red = r, .green = g, .blue = b };
             } )
           | std::ranges::to<std::vector<Palette>>();
}

} // namespace PLTE

namespace cHRM
{

ChrmChunkPayload::ChrmChunkPayload( const std::span<const std::byte> & raw_data ) :
    PngChunkPayloadBase() {
    assert( raw_data.size() == size() );

    constexpr std::size_t field_size{ sizeof( std::uint32_t ) };
    const auto read_field = [&raw_data]<std::size_t Offset>() {
        return span_to_integer<std::uint32_t, std::endian::big,
                               std::endian::native>(
            raw_data.subspan( Offset, field_size ) );
    };

    m_white_point_x = read_field.operator()<0>();
    m_white_point_y = read_field.operator()<field_size>();
    m_red_x = read_field.operator()<field_size * 2>();
    m_red_y = read_field.operator()<field_size * 3>();
    m_green_x = read_field.operator()<field_size * 4>();
    m_green_y = read_field.operator()<field_size * 5>();
    m_blue_x = read_field.operator()<field_size * 6>();
    m_blue_y = read_field.operator()<field_size * 7>();
}

ChrmChunkPayload::ChrmChunkPayload( ChrmChunkPayload && other ) noexcept :
    PngChunkPayloadBase(),
    m_white_point_x( other.whitePointX() ),
    m_white_point_y( other.whitePointY() ),
    m_red_x( other.redX() ),
    m_red_y( other.redY() ),
    m_green_x( other.greenX() ),
    m_green_y( other.greenY() ),
    m_blue_x( other.blueX() ),
    m_blue_y( other.blueY() ) {
    other.setInvalid();
}

ChrmChunkPayload &
ChrmChunkPayload::operator=( ChrmChunkPayload && other ) noexcept {
    if ( this != &other ) {
        PngChunkPayloadBase::operator=( std::move( other ) );

        m_white_point_x = other.m_white_point_x;
        m_white_point_y = other.m_white_point_y;
        m_red_x = other.m_red_x;
        m_red_y = other.m_red_y;
        m_green_x = other.m_green_x;
        m_green_y = other.m_green_y;
        m_blue_x = other.m_blue_x;
        m_blue_y = other.m_blue_y;

        other.setInvalid();
    }

    return *this;
}

} // namespace cHRM

namespace gAMA
{

GamaChunkPayload::GamaChunkPayload( const std::span<const std::byte> & raw_data ) :
    PngChunkPayloadBase(),
    m_image_gamma( 0 ) {
    assert( raw_data.size() == size() );
    m_image_gamma = span_to_integer<std::uint32_t, std::endian::big,
                                    std::endian::native>( raw_data );
}

GamaChunkPayload::GamaChunkPayload( GamaChunkPayload && other ) noexcept :
    PngChunkPayloadBase(),
    m_image_gamma( other.imageGamma() ) {
    other.setInvalid();
}

GamaChunkPayload &
GamaChunkPayload::operator=( GamaChunkPayload && other ) noexcept {
    if ( this != &other ) {
        PngChunkPayloadBase::operator=( std::move( other ) );
        m_image_gamma = other.m_image_gamma;
        other.setInvalid();
    }

    return *this;
}

} // namespace gAMA

namespace sRGB
{

SrgbChunkPayload::SrgbChunkPayload( const std::span<const std::byte> & raw_data ) :
    PngChunkPayloadBase(),
    m_rendering_intent( RenderingIntent::INVALID ) {
    assert( raw_data.size() == size() );
    m_rendering_intent = static_cast<RenderingIntent>(
        span_to_integer<std::uint8_t, std::endian::big, std::endian::native>(
            raw_data ) );
}

SrgbChunkPayload::SrgbChunkPayload( SrgbChunkPayload && other ) noexcept :
    PngChunkPayloadBase(),
    m_rendering_intent( other.renderingIntent() ) {
    other.setInvalid();
}

SrgbChunkPayload &
SrgbChunkPayload::operator=( SrgbChunkPayload && other ) noexcept {
    if ( this != &other ) {
        PngChunkPayloadBase::operator=( std::move( other ) );
        m_rendering_intent = other.m_rendering_intent;
        other.setInvalid();
    }

    return *this;
}

} // namespace sRGB


} // namespace PNG
