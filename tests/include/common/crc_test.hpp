#pragma once

#include "common/crc.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string_view>

namespace CRC_TEST
{

constexpr std::string
get_file_data( const std::filesystem::directory_entry & data_src,
               const std::streamsize                    expected_size = 0 ) {
    std::error_code err_code{};
    if ( !data_src.exists( err_code ) ) {
        throw std::filesystem::filesystem_error(
            data_src.path().string() + " does not exist.", err_code );
    }

    std::ifstream file{ data_src.path(), std::ios::in };
    if ( !file.is_open() ) {
        throw std::runtime_error( "Unable to open file." );
    }

    file.seekg( 0, std::ios_base::end );
    const auto file_size{ file.tellg() };
    if ( file.fail() ) {
        throw std::runtime_error( "Unable to read file size." );
    }
    file.clear();
    file.seekg( 0, std::ios_base::beg );

    if ( expected_size != 0 && expected_size != file_size ) {
        throw std::runtime_error(
            "Size mismatch between file length & expected size." );
    }

    std::string file_buffer( static_cast<std::size_t>( file_size ), '\0' );
    file.read( file_buffer.data(), file_size );

    if ( !file.good() ) {
        throw std::runtime_error( "Failed to read file to string buffer." );
    }

    return file_buffer;
}

constexpr auto
get_crc( const std::span<const std::byte> data_stream ) {
    CRC::CrcTable32 crc_calculator(
        CRC::PNG::png_polynomial<std::endian::big>() );
    return crc_calculator.crc( data_stream );
}

constexpr auto
get_crc( const std::string_view data_stream ) {
    return get_crc(
        std::as_bytes( std::span{ data_stream.data(), data_stream.size() } ) );
}

constexpr auto
get_crc( const std::filesystem::directory_entry & data_src,
         const std::streamsize                    expected_size ) {
    const auto file_buf = get_file_data( data_src, expected_size );

    return get_crc( std::string_view{ file_buf.data(), file_buf.size() } );
}

} // namespace CRC_TEST

int crc_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv );
