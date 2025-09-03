#include "common/crc_test.hpp"

#include <cassert>
#include <iostream>

namespace CRC_TEST
{

constexpr auto
test_iend( const std::filesystem::directory_entry & data_src ) {
    return TEST_INTERFACE::test_function(
        get_crc, std::bitset<CRC::crc_bits>{ 0xAE426082 }, data_src,
        std::streamsize{ 4 } );
}

constexpr bool
test_crc_iend( const std::filesystem::directory_entry & data_src ) {
    const auto file_buf = get_file_data( data_src, 4 );

    const std::span<const std::byte> data_stream{
        reinterpret_cast<const std::byte *>( file_buf /*.view()*/.data() ),
        /*data_sz*/ file_buf.size()
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

} // namespace CRC_TEST

int
crc_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    const int n_tests{ 1 };
    int       test_passes{ 0 };

    const std::filesystem::directory_entry iend_file{
        std::filesystem::current_path() / ".." / ".." / ".." / "tests" / "data"
        / "iend_bytes.bin"
    };
    test_passes += ( CRC_TEST::test_iend( iend_file ) ? 1 : 0 );

    return n_tests - test_passes;
}
