#include "png/png.hpp"

#include <bit>
#include <cassert>
// #include <iomanip>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>

namespace PNG
{

PNG::PNG( const std::string_view raw_data ) :
    valid_png( true ),
    current_error( png_error_t::NONE ),
    header_bytes( 0 ),
    png_chunks( 0 ),
    crc_calculator( CRC::PNG::png_polynomial<std::endian::big>() ) {
    // Check first 8 bytes for valid header type
    assert( raw_data.size() >= header_bits );

    // Verify valid png header
    header_bytes = std::bitset<header_bits>{
        convert_endian<std::endian::big, std::endian::little, std::uint64_t>(
            *reinterpret_cast<const std::uint64_t *>( raw_data.data() ) )
    };
    if ( !( valid_png = verify_header() ) ) {
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
    const std::uint32_t data_size{ convert_endian<std::endian::big>(
        *reinterpret_cast<const std::uint32_t *>( chunk_data.data()
                                                  + data_offset ) ) };
    data_offset += 4;

    // Parse chunk type
    const png_chunk_t potential_chunk_type{ static_cast<png_chunk_t>(
        convert_endian<std::endian::big>(
            *reinterpret_cast<const std::uint32_t *>( chunk_data.data()
                                                      + data_offset ) ) ) };

    // Get start pointer for validating CRC
    const auto crc_start_ptr{ reinterpret_cast<const std::byte *>(
        chunk_data.data() + data_offset ) };
    if ( potential_chunk_type == png_chunk_t::IEND ) {
        const auto data_path{ std::filesystem::current_path() / ".." / "tests"
                              / "data" / "iend_bytes.bin" };
        std::ofstream out_file{ data_path,
                                std::ios_base::out | std::ios_base::binary };
        if ( !out_file.is_open() ) {
            std::cerr << "Failed to open iend out file" << std::endl;
            exit( 1 );
        }

        std::cout << std::string{ chunk_data.data() + data_offset, 4 }
                  << std::endl;
        out_file.write( chunk_data.data() + data_offset, 4 );
    }
    data_offset += 4;

    // Copy data
    auto data{ std::make_unique<std::byte *>( new std::byte[data_size] ) };
    std::copy_n(
        reinterpret_cast<const std::byte *>( chunk_data.data() + data_offset ),
        data_size,
        *data );
    data_offset += data_size;

    // Parse CRC
    const std::bitset<CRC::crc_bits> parsed_crc{ std::byteswap(
        *reinterpret_cast<const std::uint32_t *>( chunk_data.data()
                                                  + data_offset ) ) };
    data_offset += 4;

    // Validate CRC
    const auto crc{ crc_calculator.crc(
        std::span{ crc_start_ptr, 4 + data_size } ) };

    return PNGChunk( data_size,
                     PNGChunk::is_valid( potential_chunk_type ) ?
                         potential_chunk_type :
                         png_chunk_t::INVALID,
                     std::move( data ),
                     crc );
}

} // namespace PNG
