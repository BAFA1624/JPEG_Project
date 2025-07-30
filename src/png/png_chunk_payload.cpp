#include "png/png_chunk_payload.hpp"

namespace PNG
{

constexpr IhdrChunkPayload::IhdrChunkPayload(
    const std::span<const std::byte> & raw_data ) {
    // Parsing in data members:
    constexpr std::size_t width_offset{ 0 + sizeof( m_width ) };
    m_width =
        span_to_integer<std::uint32_t, std::endian::big, std::endian::native>(
            raw_data.subspan( 0, width_offset ) );

    constexpr std::size_t height_offset{ width_offset + sizeof( m_height ) };
    m_height =
        span_to_integer<std::uint32_t, std::endian::big, std::endian::native>(
            raw_data.subspan( width_offset, height_offset ) );
}

} // namespace PNG
