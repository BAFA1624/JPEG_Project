#pragma once

#include "common/crc.hpp"
#include "test_interface.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <stdexcept>

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
get_crc( const std::filesystem::directory_entry & data_src,
         const std::streamsize                    expected_size ) {
    const auto file_buf = get_file_data( data_src, expected_size );

    const std::span<const std::byte> data_stream{
        reinterpret_cast<const std::byte *>( file_buf.data() ), file_buf.size()
    };

    CRC::CrcTable32 crc_calculator(
        CRC::PNG::png_polynomial<std::endian::big>() );

    return crc_calculator.crc( data_stream );
}

} // namespace CRC_TEST
