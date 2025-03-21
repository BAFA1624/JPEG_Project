#include "png/png.hpp"

#include <bit>
#include <cassert>
// #include <iomanip>
#include <iostream>
#include <span>
// #include <ranges>

namespace PNG
{

PNG::PNG( const std::string_view raw_data ) :
    valid_png( true ),
    current_error( png_error_t::NONE ),
    header_bits( 0 ),
    png_chunks( 0 ),
    crc_calculator( CRC::PNG::png_polynomial<std::endian::big>() ) {
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
        // std::cout << png_chunks[png_chunks.size() - 1] << std::endl;
    } while ( data_offset < raw_data.size() );

    std::cout << "Big endian:    "
              << CRC::PNG::png_polynomial<std::endian::big>() << std::endl;
    std::cout << "Little endian: "
              << CRC::PNG::png_polynomial<std::endian::little>() << std::endl;
    std::cout << "Big endian unswapped: " << std::hex
              << CRC::PNG::png_polynomial<std::endian::big>().to_ullong()
              << std::endl;
    std::cout << "Big endian swapped: " << std::hex
              << byteswap(
                     CRC::PNG::png_polynomial<std::endian::big>().to_ullong() )
              << std::endl;
    std::cout << "Little endian unswapped: " << std::hex
              << CRC::PNG::png_polynomial<std::endian::little>().to_ullong()
              << std::endl;
    std::cout << "Little endian swapped: " << std::hex
              << byteswap( CRC::PNG::png_polynomial<std::endian::little>()
                               .to_ullong() )
              << std::endl;

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

    // Get start pointer for validating CRC
    const auto crc_start_ptr{ reinterpret_cast<const std::byte *>(
        chunk_data.data() + data_offset ) };
    data_offset += 4;

    // Copy data
    auto data{ std::make_unique<std::byte *>( new std::byte[data_size] ) };
    std::copy_n(
        reinterpret_cast<const std::byte *>( chunk_data.data() + data_offset ),
        data_size,
        *data );
    // auto data{ new char[data_size] };
    // std::copy_n( chunk_data.data() + data_offset, data_size, data );
    data_offset += data_size;

    // Parse CRC
    const std::bitset<32> parsed_crc{ *reinterpret_cast<const std::uint32_t *>(
        chunk_data.data() + data_offset ) };
    data_offset += 4;

    // Validate CRC
    // TODO
    const auto crc{ crc_calculator.crc(
        std::span{ crc_start_ptr, 4 + data_size } ) };
    const auto crc2{ crc_calculator.crc2(
        std::span{ crc_start_ptr, 4 + data_size } ) };

    if ( ( parsed_crc ^ crc ).any() ) {
        std::cout << "Difference in crc detected:\n";
        std::cout << "Parsed crc:        " << parsed_crc << std::endl;
        std::cout << "1) Calculated crc: " << crc << std::endl;
        std::cout << "   Difference:     " << ( parsed_crc ^ crc ) << std::endl;
        std::cout << "2) Calculated crc: " << crc2 << std::endl;
        std::cout << "   Difference:     " << ( parsed_crc ^ crc2 )
                  << std::endl;
    }
    else {
        std::cout << "CRC VALIDATED!\n";
    }


    return PNGChunk( data_size,
                     PNGChunk::is_valid( potential_chunk_type ) ?
                         potential_chunk_type :
                         png_chunk_t::INVALID,
                     std::move( data ),
                     crc );
}

} // namespace PNG
