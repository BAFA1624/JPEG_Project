#include "common/crc_test.hpp"

#include "common/test_interface.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <ranges>
#include <vector>

namespace CRC_TEST
{

constexpr bool
test_empty() {
    constexpr std::bitset<CRC::crc_bits> empty_crc{ 0x00000000 };
    constexpr auto crc_from_string = []( const std::string_view data ) {
        return get_crc( data );
    };
    return TEST_INTERFACE::test_function(
        crc_from_string, empty_crc, std::string_view{} );
}

constexpr bool
test_ascii_digits() {
    constexpr std::bitset<CRC::crc_bits> digits_crc{ 0xCBF43926 };
    constexpr auto crc_from_string = []( const std::string_view data ) {
        return get_crc( data );
    };
    return TEST_INTERFACE::test_function(
        crc_from_string, digits_crc, std::string_view{ "123456789" } );
}

constexpr bool
test_iend( const std::filesystem::directory_entry & data_src ) {
    constexpr std::bitset<CRC::crc_bits> iend_crc{ 0xAE426082 };
    constexpr auto                       crc_from_file =
        []( const std::filesystem::directory_entry & input_file,
            const std::streamsize                    expected_size ) {
            return get_crc( input_file, expected_size );
        };
    return TEST_INTERFACE::test_function(
        crc_from_file, iend_crc, data_src, std::streamsize{ 4 } );
}

constexpr bool
test_ihdr_bytes() {
    constexpr std::bitset<CRC::crc_bits> ihdr_crc{ 0x28CB34BB };
    constexpr auto                       ihdr_bytes =
        std::array{ std::byte{ 0x49 }, std::byte{ 0x48 }, std::byte{ 0x44 },
                    std::byte{ 0x52 }, std::byte{ 0x00 }, std::byte{ 0x00 },
                    std::byte{ 0x00 }, std::byte{ 0x01 }, std::byte{ 0x00 },
                    std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x01 },
                    std::byte{ 0x08 }, std::byte{ 0x03 }, std::byte{ 0x00 },
                    std::byte{ 0x00 }, std::byte{ 0x00 } };
    constexpr auto crc_from_bytes =
        []( const std::span<const std::byte> data ) { return get_crc( data ); };
    return TEST_INTERFACE::test_function(
        crc_from_bytes, ihdr_crc, std::span<const std::byte>{ ihdr_bytes } );
}

const auto crc_test_functions =
    std::vector{ test_empty, test_ascii_digits, test_ihdr_bytes };

} // namespace CRC_TEST

int
crc_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t n_tests = 0;
    std::size_t test_passes = 0;

    n_tests += CRC_TEST::crc_test_functions.size();
    for ( const auto & func : CRC_TEST::crc_test_functions ) {
        test_passes += ( func() ? 1 : 0 );
    }

    test_passes +=
        ( CRC_TEST::test_iend( std::filesystem::directory_entry{
              std::filesystem::path{ __FILE__ }.parent_path().parent_path()
              / "data" / "iend_bytes.bin" } ) ?
              1 :
              0 );
    ++n_tests;

    return static_cast<int>( n_tests - test_passes );
}
