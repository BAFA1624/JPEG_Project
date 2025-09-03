#include "common/crc_test.hpp"

#include <cassert>
#include <iostream>
#include <ranges>

namespace CRC_TEST
{

constexpr bool
test_iend( const std::filesystem::directory_entry & data_src ) {
    constexpr std::bitset<CRC::crc_bits> iend_crc{ 0xAE426082 };
    return TEST_INTERFACE::test_function(
        get_crc, iend_crc, data_src, std::streamsize{ 4 } );
}

} // namespace CRC_TEST

int
crc_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t n_tests = 0;
    std::size_t test_passes = 0;

    const auto crc_test_inputs = std::vector{ std::filesystem::directory_entry{
        std::filesystem::current_path() / ".." / ".." / ".." / "tests" / "data"
        / "iend_bytes.bin" } };

    n_tests += CRC_TEST::crc_test_functions.size();
    for ( const auto & [func, input] : std::ranges::views::zip(
              CRC_TEST::crc_test_functions, crc_test_inputs ) ) {
        test_passes += ( func( input ) ? 1 : 0 );
    }

    return static_cast<int>( n_tests - test_passes );
}
