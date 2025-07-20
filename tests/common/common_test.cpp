#include "common/common_test.hpp"

#include <format>
#include <functional>
#include <iostream>
#include <random>

constexpr bool
test_convert_endian() {
    constexpr auto test_little_little{
        convert_endian<std::endian::little, std::endian::little>( 0x12345678u )
        == 0x12345678u
    };
    if ( !test_little_little ) {
        std::cerr << "convert_endian<std::endian::little, std::endian::little> "
                     "failed."
                  << std::endl;
    }

    constexpr auto test_little_big{
        convert_endian<std::endian::little, std::endian::big>( 0x12345678u )
        == 0x78563412u
    };
    if ( !test_little_big ) {
        std::cerr
            << "convert_endian<std::endian::little, std::endian::big> failed."
            << std::endl;
    }

    constexpr auto test_big_little{
        convert_endian<std::endian::big, std::endian::little>( 0x78563412u )
        == 0x12345678u
    };
    if ( !test_big_little ) {
        std::cerr
            << "convert_endian<std::endian::big, std::endian::little> failed."
            << std::endl;
    }

    constexpr auto test_big_big{
        convert_endian<std::endian::big, std::endian::big>( 0x78563412u )
        == 0x78563412u
    };
    if ( !test_big_big ) {
        std::cerr
            << "convert_endian<std::endian::big, std::endian::big> failed."
            << std::endl;
    }

    return test_little_little && test_little_big && test_big_little
           && test_big_big;
}

constexpr bool
test_lsb_msb() {
    constexpr std::size_t N{ 32 };

    constexpr auto test_little_lsb_32{ lsb<std::endian::little, N> == N - 1 };
    if ( !test_little_lsb_32 ) {
        std::cerr << std::format( "lsb<std::endian::little, {}> failed.", N )
                  << std::endl;
    }
    constexpr auto test_little_msb_32{ msb<std::endian::little, N> == 0 };
    if ( !test_little_msb_32 ) {
        std::cerr << std::format( "msb<std::endian::little, {}> failed.", N )
                  << std::endl;
    }

    constexpr auto test_big_lsb_32{ lsb<std::endian::big, N> == 0 };
    if ( !test_big_lsb_32 ) {
        std::cerr << std::format( "lsb<std::endian::big, {}> failed.", N )
                  << std::endl;
    }
    constexpr auto test_big_msb_32{ msb<std::endian::big, N> == N - 1 };
    if ( !test_big_msb_32 ) {
        std::cerr << std::format( "msb<std::endian::big, {}> failed.", N )
                  << std::endl;
    }

    return test_little_lsb_32 && test_little_msb_32 && test_big_lsb_32
           && test_big_msb_32;
}

constexpr bool
test_lsb_msb_offset() {
    constexpr std::size_t N{ 32 };

    constexpr auto test_little_lsb_offset_0{
        lsb_offset<std::endian::little, 0, N>() == N - 1
    };
    if ( !test_little_lsb_offset_0 ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, 0, {}> failed.", N )
                  << std::endl;
    }

    constexpr auto test_little_lsb_offset_max{
        lsb_offset<std::endian::little, 31, 32>() == 0
    };
    if ( !test_little_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, {}, {}> failed.", N - 1, N )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_0{
        msb_offset<std::endian::little, 0, N>() == N - 1
    };
    if ( !test_little_msb_offset_0 ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, 0, {}> failed.", N )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_max{
        msb_offset<std::endian::little, N - 1, N>() == 0
    };
    if ( !test_little_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, {}, {}> failed.", N - 1, N )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_0{ lsb_offset<std::endian::big, 0, N>()
                                          == 0 };
    if ( !test_big_lsb_offset_0 ) {
        std::cerr << std::format( "lsb_offset<std::endian::big, 0, {}> failed.",
                                  N )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_max{
        lsb_offset<std::endian::big, N - 1, N>() == N - 1
    };
    if ( !test_big_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::big, {}, {}> failed.", N - 1, N )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_0{ msb_offset<std::endian::big, 0, N>()
                                          == 0 };
    if ( !test_big_msb_offset_0 ) {
        std::cerr << std::format( "msb_offset<std::endian::big, 0, {}> failed.",
                                  N )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_max{
        msb_offset<std::endian::big, N - 1, N>() == N - 1
    };
    if ( !test_big_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::big, {}, {}> failed.", N - 1, N )
                  << std::endl;
    }

    return test_little_lsb_offset_0 && test_little_lsb_offset_max
           && test_little_msb_offset_0 && test_little_msb_offset_max
           && test_big_lsb_offset_0 && test_big_lsb_offset_max
           && test_big_msb_offset_0 && test_big_msb_offset_max;
}

bool
test_span_to_integer() {
    std::random_device r{};

    std::uniform_int_distribution<std::uint8_t> uniform_uint8_dist(
        std::numeric_limits<std::uint8_t>::min(),
        std::numeric_limits<std::uint8_t>::max() );
    std::uniform_int_distribution<std::uint16_t> uniform_uint16_dist(
        std::numeric_limits<std::uint16_t>::min(),
        std::numeric_limits<std::uint16_t>::max() );
    std::uniform_int_distribution<std::uint32_t> uniform_uint32_dist(
        std::numeric_limits<std::uint32_t>::min(),
        std::numeric_limits<std::uint32_t>::max() );
    std::uniform_int_distribution<std::uint64_t> uniform_uint64_dist(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max() );
#ifdef __SIZEOF_INT128__
    std::uniform_int_distribution<__uint128_t> uniform_uint128_dist(
        static_cast<__uint128_t>( 0 ), static_cast<__uint128_t>( ~0 ) );
#endif

    // Integers for tests
    const std::uint8_t  uint_8{ uniform_uint8_dist( r ) };
    const std::uint16_t uint_16{ uniform_uint16_dist( r ) };
    const std::uint32_t uint_32{ uniform_uint32_dist( r ) };
    const std::uint64_t uint_64{ uniform_uint64_dist( r ) };
#ifdef __SIZEOF_INT128__
    const __uint128_t uint_128{ uniform_uint128_dist( r ) };
#endif

    // Byte vectors for tests
    constexpr const std::uint32_t x{ 0xFF };
    const auto test = integral_to_bytes<x, std::endian::little>();
    const auto vector_8_little{
        integral_to_bytes<std::uint8_t, std::endian::little>( uint_8 )
    };

    // Test cases:
    // 1 byte uinteger:
    // auto test_1_byte_uint_little{};
    // if ( !test_1_byte_uint_little ) {}
    // auto test_1_byte_uint_big{};
    // if ( !test_1_byte_uint_big ) {}
    //// 2 byte uinteger:
    // auto test_2_byte_uint_little{};
    // if ( !test_2_byte_uint_little ) {}
    // auto test_2_byte_uint_big{};
    // if ( !test_2_byte_uint_little ) {}
    //// 4 byte uinteger:
    // auto test_4_byte_uint_little{};
    // if ( !test_4_byte_uint_little ) {}
    // auto test_4_byte_uint_big{};
    // if ( !test_4_byte_uint_big ) {}
    //// 8 byte uinteger:
    // auto test_8_byte_uint_little{};
    // if ( !test_8_byte_uint_little ) {}
    // auto test_8_byte_uint_big{};
    // if ( !test_8_byte_uint_big ) {}
#ifdef __SIZEOF_INT128__
    // Generic case:
    // auto test_large_uint_little{};
    // if ( !test_large_uint_little ) {}
    // auto test_large_uint_big{};
    // if ( !test_large_uint_big ) {}
#endif
}

const auto test_functions = std::vector<std::function<bool()>>{
    test_convert_endian, test_lsb_msb,
    test_lsb_msb_offset /*, test_span_to_integer*/
};

int
common_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t test_passes{ 0 };

    for ( const auto & test : test_functions )
        test_passes += ( test() ? 1 : 0 );

    return test_functions.size() - test_passes;
}
