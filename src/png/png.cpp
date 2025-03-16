#include "png/png.hpp"

#include <bit>
#include <cassert>
#include <iomanip> #include <iostream>
#include <ranges>

namespace PNG
{

PNG::PNG( const std::string_view raw_data ) :
    valid_png( true ),
    current_error( png_error_t::NONE ),
    header_bits( 0 ),
    png_chunks( 0 ) {
    // Check first 8 bytes for valid header type
    assert( raw_data.size() >= 8 );

    // Verify valid png header
    header_bits = std::bitset<64>{ byteswap(
        *reinterpret_cast<const std::uint64_t *>( raw_data.data() ) ) };
    if ( !( valid_png = verify_header() ) ) {
        valid_png = false;
        throw bad_png_header();
    }

    // Maintains current offset in the raw data
    std::size_t data_offset{ 8 };

    // Read PNG blocks
    png_chunks.reserve( 10 );
    do {
        png_chunks.emplace_back( parse_chunk( raw_data, data_offset ) );
        std::cout << png_chunks[png_chunks.size() - 1] << std::endl;
    } while ( data_offset < raw_data.size() );

    png_chunks.shrink_to_fit();
}

[[nodiscard]] PNGChunk
PNG::parse_chunk( const std::string_view chunk_data,
                  std::size_t &          data_offset ) {
    // Parse size of block data
    const std::uint32_t data_size{ byteswap(
        *reinterpret_cast<const std::uint32_t *>( chunk_data.data()
                                                  + data_offset ) ) };
    data_offset += 4;

    // Parse chunk type
    const png_chunk_t potential_chunk_type{ static_cast<png_chunk_t>(
        byteswap( *reinterpret_cast<const std::uint32_t *>(
            chunk_data.data() + data_offset ) ) ) };
    data_offset += 4;

    // Copy data
    auto data{ std::make_unique<char *>( new char[data_size] ) };
    std::copy_n( chunk_data.data() + data_offset, data_size, *data );
    // auto data{ new char[data_size] };
    // std::copy_n( chunk_data.data() + data_offset, data_size, data );
    data_offset += data_size;

    // Parse CRC
    const std::bitset<32> parsed_crc{ byteswap(
        *reinterpret_cast<const std::uint32_t *>( chunk_data.data()
                                                  + data_offset ) ) };
    data_offset += 4;

    // Validate CRC
    // TODO
    const auto crc = parsed_crc;

    return PNGChunk( data_size,
                     PNGChunk::is_valid( potential_chunk_type ) ?
                         potential_chunk_type :
                         png_chunk_t::INVALID,
                     std::move( data ),
                     crc );
}

} // namespace PNG
