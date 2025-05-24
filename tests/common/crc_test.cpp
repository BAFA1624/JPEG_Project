#include "common/crc.hpp"
// #include "common/test_interfaces.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

constexpr bool
test_crc_iend( const std::filesystem::directory_entry & data_src ) {
    if ( !data_src.exists() ) {
        std::cerr << data_src.path().string() << " does not exist."
                  << std::endl;
        return false;
    }
    assert( data_src.exists() );

    std::ifstream iend_sample_file{ data_src.path(), std::ios::in };
    if ( !iend_sample_file.is_open() ) {
        std::cerr << "Unable to open file: " << data_src.path().string()
                  << std::endl;
        return false;
    }

    iend_sample_file.seekg( 0, std::ios_base::end );
    const auto data_sz{ static_cast<std::size_t>( iend_sample_file.tellg() ) };
    iend_sample_file.clear();
    iend_sample_file.seekg( 0, std::ios_base::beg );

    if ( data_sz != 4 ) {
        std::cerr << "IEND data should be 4 bytes in size. Parsed " << data_sz
                  << " bytes." << std::endl;
        return false;
    }

    std::stringstream file_buf;
    file_buf << iend_sample_file.rdbuf();

    const std::span<const std::byte> data_stream{
        reinterpret_cast<const std::byte *>( file_buf.view().data() ), data_sz
    };

    CRC::CrcTable32 crc_calculator(
        CRC::PNG::png_polynomial<std::endian::big>() );

    const auto calculated_crc{ crc_calculator.crc( data_stream ) };

    constexpr std::bitset<CRC::crc_bits> true_iend_crc{ 0xAE426082 };

    if ( calculated_crc != true_iend_crc ) {
        std::cerr << std::hex
                  << std::format(
                         "Calculated crc ({:X}) does not match expected value "
                         "({:X}).",
                         calculated_crc.to_ulong(), true_iend_crc.to_ulong() )
                  << std::dec << std::endl;
        return false;
    }

    return true;
}

int
crc_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    const int n_tests{ 1 };
    int       test_passes{ 0 };

    const std::filesystem::directory_entry iend_file{
        std::filesystem::current_path() / ".." / ".." / ".." / "tests" / "data"
        / "iend_bytes.bin"
    };
    test_passes += ( test_crc_iend( iend_file ) ? 1 : 0 );

    return n_tests - test_passes;
}
