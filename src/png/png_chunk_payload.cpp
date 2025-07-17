#include "png/png_chunk_payload.hpp"

namespace PNG
{

IhdrChunkPayload::IhdrChunkPayload(
    const std::span<const std::byte> & raw_data ) {
    std::uint32_t data_position{ 0 };

    // Parsing in data members:
    m_width = to_integer<std::uint32_t>( raw_data.subspan( data_position, 4 ) );
    data_position += 4;
}

} // namespace PNG
