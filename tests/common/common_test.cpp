#include "common/common_test.hpp"

#include <format>
#include <random>

namespace COMMON_TEST
{

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
    constexpr std::uint32_t size{ sizeof( size ) * byte_bits };

    constexpr auto test_little_lsb_32{
        lsb<decltype( size ), std::endian::little> == 0
    };
    if ( !test_little_lsb_32 ) {
        std::println( std::cerr,
                      "lsb<std::uint32_t, std::endian::little> failed." );
    }
    constexpr auto test_little_msb_32{
        msb<decltype( size ), std::endian::little> == size - 1
    };
    if ( !test_little_msb_32 ) {
        std::println( std::cerr,
                      "msb<std::uint32_t, std::endian::little> failed." );
    }

    constexpr auto test_big_lsb_32{ lsb<decltype( size ), std::endian::big>
                                    == size - 1 };
    if ( !test_big_lsb_32 ) {
        std::println( std::cerr,
                      "lsb<std::uint32_t, std::endian::big> failed." );
    }
    constexpr auto test_big_msb_32{ msb<decltype( size ), std::endian::big>
                                    == 0 };
    if ( !test_big_msb_32 ) {
        std::println( std::cerr,
                      "msb<std::uint32_t, std::endian::big> failed." );
    }

    return test_little_lsb_32 && test_little_msb_32 && test_big_lsb_32
           && test_big_msb_32;
}

constexpr bool
test_lsB_msB() {
    constexpr std::uint32_t size{ sizeof( size ) };

    constexpr auto test_little_lsB_32{
        lsB<decltype( size ), std::endian::little> == 0
    };
    if ( !test_little_lsB_32 ) {
        std::println( std::cerr,
                      "lsB<std::uint32_t, std::endian::little> failed." );
    }
    constexpr auto test_little_msB_32{
        msB<decltype( size ), std::endian::little> == size - 1
    };
    if ( !test_little_msB_32 ) {
        std::println( std::cerr,
                      "msB<std::uint32_t, std::endian::little> failed." );
    }

    constexpr auto test_big_lsB_32{ lsB<decltype( size ), std::endian::big>
                                    == size - 1 };
    if ( !test_big_lsB_32 ) {
        std::println( std::cerr,
                      "lsB<std::uint32_t, std::endian::big> failed." );
    }
    constexpr auto test_big_msB_32{ msB<decltype( size ), std::endian::big>
                                    == 0 };
    if ( !test_big_msB_32 ) {
        std::println( std::cerr,
                      "msB<std::uint32_t, std::endian::big> failed." );
    }

    return test_little_lsB_32 && test_little_msB_32 && test_big_lsB_32
           && test_big_msB_32;
}

constexpr bool
test_lsb_msb_offset_consteval() {
    using T = std::uint32_t;
    constexpr T size{ 32 };

    constexpr auto test_little_lsb_offset_0{
        lsb_offset<std::endian::little, T, 0>() == 0
    };
    if ( !test_little_lsb_offset_0 ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, 0, {}> failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_lsb_offset_max{
        lsb_offset<std::endian::little, T, 31>() == size - 1
    };
    if ( !test_little_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_0{
        msb_offset<std::endian::little, T, 0>() == size - 1
    };
    if ( !test_little_msb_offset_0 ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, 0, {}> failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_max{
        msb_offset<std::endian::little, T, size - 1>() == 0
    };
    if ( !test_little_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_0{ lsb_offset<std::endian::big, T, 0>()
                                          == size - 1 };
    if ( !test_big_lsb_offset_0 ) {
        std::cerr << std::format( "lsb_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_max{
        lsb_offset<std::endian::big, T, size - 1>() == 0
    };
    if ( !test_big_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_0{ msb_offset<std::endian::big, T, 0>()
                                          == 0 };
    if ( !test_big_msb_offset_0 ) {
        std::cerr << std::format( "msb_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_max{
        msb_offset<std::endian::big, T, size - 1>() == size - 1
    };
    if ( !test_big_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    return test_little_lsb_offset_0 && test_little_lsb_offset_max
           && test_little_msb_offset_0 && test_little_msb_offset_max
           && test_big_lsb_offset_0 && test_big_lsb_offset_max
           && test_big_msb_offset_0 && test_big_msb_offset_max;
}

constexpr bool
test_lsb_msb_offset_constexpr() {
    using T = std::uint32_t;
    constexpr T size{ 32 };

    constexpr auto test_little_lsb_offset_0{
        lsb_offset<std::endian::little, T>( 0 ) == 0
    };
    if ( !test_little_lsb_offset_0 ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, 0>({}) failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_lsb_offset_max{
        lsb_offset<std::endian::little, T>( size - 1 ) == size - 1
    };
    if ( !test_little_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_0{
        msb_offset<std::endian::little, T, 0>() == size - 1
    };
    if ( !test_little_msb_offset_0 ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, 0, {}> failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_max{
        msb_offset<std::endian::little, T, size - 1>() == 0
    };
    if ( !test_little_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_0{ lsb_offset<std::endian::big, T, 0>()
                                          == size - 1 };
    if ( !test_big_lsb_offset_0 ) {
        std::cerr << std::format( "lsb_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_max{
        lsb_offset<std::endian::big, T, size - 1>() == 0
    };
    if ( !test_big_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_0{ msb_offset<std::endian::big, T, 0>()
                                          == 0 };
    if ( !test_big_msb_offset_0 ) {
        std::cerr << std::format( "msb_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_max{
        msb_offset<std::endian::big, T, size - 1>() == size - 1
    };
    if ( !test_big_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    return test_little_lsb_offset_0 && test_little_lsb_offset_max
           && test_little_msb_offset_0 && test_little_msb_offset_max
           && test_big_lsb_offset_0 && test_big_lsb_offset_max
           && test_big_msb_offset_0 && test_big_msb_offset_max;
}

constexpr bool
test_lsB_msB_offset_consteval() {
    using T = std::uint32_t;
    constexpr T size{ sizeof( T ) };

    constexpr auto test_little_lsB_offset_0{
        lsB_offset<std::endian::little, T, 0>() == 0
    };
    if ( !test_little_lsB_offset_0 ) {
        std::cerr << std::format(
            "lsB_offset<std::endian::little, 0, {}> failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_lsB_offset_max{
        lsB_offset<std::endian::little, T, size - 1>() == size - 1
    };
    if ( !test_little_lsB_offset_max ) {
        std::cerr << std::format(
            "lsB_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_little_msB_offset_0{
        msB_offset<std::endian::little, T, 0>() == size - 1
    };
    if ( !test_little_msB_offset_0 ) {
        std::cerr << std::format(
            "msB_offset<std::endian::little, 0, {}> failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_msB_offset_max{
        msB_offset<std::endian::little, T, size - 1>() == 0
    };
    if ( !test_little_msB_offset_max ) {
        std::cerr << std::format(
            "msB_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsB_offset_0{ lsB_offset<std::endian::big, T, 0>()
                                          == size - 1 };
    if ( !test_big_lsB_offset_0 ) {
        std::cerr << std::format( "lsB_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsB_offset_max{
        lsB_offset<std::endian::big, T, size - 1>() == 0
    };
    if ( !test_big_lsB_offset_max ) {
        std::cerr << std::format(
            "lsB_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_big_msB_offset_0{ msB_offset<std::endian::big, T, 0>()
                                          == 0 };
    if ( !test_big_msB_offset_0 ) {
        std::cerr << std::format( "msB_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    constexpr auto test_big_msB_offset_max{
        msB_offset<std::endian::big, T, size - 1>() == size - 1
    };
    if ( !test_big_msB_offset_max ) {
        std::cerr << std::format(
            "msB_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    return test_little_lsB_offset_0 && test_little_lsB_offset_max
           && test_little_msB_offset_0 && test_little_msB_offset_max
           && test_big_lsB_offset_0 && test_big_lsB_offset_max
           && test_big_msB_offset_0 && test_big_msB_offset_max;
}

constexpr bool
test_lsB_msB_offset_constexpr() {
    using T = std::uint32_t;
    constexpr T size{ 32 };

    constexpr auto test_little_lsb_offset_0{
        lsb_offset<std::endian::little, T>( 0 ) == 0
    };
    if ( !test_little_lsb_offset_0 ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, 0>({}) failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_lsb_offset_max{
        lsb_offset<std::endian::little, T>( size - 1 ) == size - 1
    };
    if ( !test_little_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_0{
        msb_offset<std::endian::little, T, 0>() == size - 1
    };
    if ( !test_little_msb_offset_0 ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, 0, {}> failed.", size )
                  << std::endl;
    }

    constexpr auto test_little_msb_offset_max{
        msb_offset<std::endian::little, T, size - 1>() == 0
    };
    if ( !test_little_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::little, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_0{ lsb_offset<std::endian::big, T, 0>()
                                          == size - 1 };
    if ( !test_big_lsb_offset_0 ) {
        std::cerr << std::format( "lsb_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    // Expect this to be wrong!
    constexpr auto test_big_lsb_offset_max{
        lsb_offset<std::endian::big, T, size - 1>() == 0
    };
    if ( !test_big_lsb_offset_max ) {
        std::cerr << std::format(
            "lsb_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_0{ msb_offset<std::endian::big, T, 0>()
                                          == 0 };
    if ( !test_big_msb_offset_0 ) {
        std::cerr << std::format( "msb_offset<std::endian::big, 0, {}> failed.",
                                  size )
                  << std::endl;
    }

    constexpr auto test_big_msb_offset_max{
        msb_offset<std::endian::big, T, size - 1>() == size - 1
    };
    if ( !test_big_msb_offset_max ) {
        std::cerr << std::format(
            "msb_offset<std::endian::big, {}, {}> failed.", size - 1, size )
                  << std::endl;
    }

    return test_little_lsb_offset_0 && test_little_lsb_offset_max
           && test_little_msb_offset_0 && test_little_msb_offset_max
           && test_big_lsb_offset_0 && test_big_lsb_offset_max
           && test_big_msb_offset_0 && test_big_msb_offset_max;
}

bool
test_span_to_integer() {
    std::random_device rand_device{};

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
        __uint128_t( 0 ), ~__uint128_t{ 0 } );
#endif

    // Integers for tests
    const std::uint8_t  uint_8{ uniform_uint8_dist( rand_device ) };
    const std::uint16_t uint_16{ uniform_uint16_dist( rand_device ) };
    const std::uint32_t uint_32{ uniform_uint32_dist( rand_device ) };
    const std::uint64_t uint_64{ uniform_uint64_dist( rand_device ) };
#ifdef __SIZEOF_INT128__
    const __uint128_t uint_128{ uniform_uint128_dist( rand_device ) };
#endif

    // Byte vectors for tests
    const auto vector_8_little{
        integral_to_bytes<std::uint8_t, std::endian::little>( uint_8 )
    };
    const auto vector_8_big{ integral_to_bytes<std::uint8_t, std::endian::big>(
        uint_8 ) };
    const auto vector_16_little{
        integral_to_bytes<std::uint16_t, std::endian::little>( uint_16 )
    };
    const auto vector_16_big{
        integral_to_bytes<std::uint16_t, std::endian::big>( uint_16 )
    };
    const auto vector_32_little{
        integral_to_bytes<std::uint32_t, std::endian::little>( uint_32 )
    };
    const auto vector_32_big{
        integral_to_bytes<std::uint32_t, std::endian::big>( uint_32 )
    };
    const auto vector_64_little{
        integral_to_bytes<std::uint64_t, std::endian::little>( uint_64 )
    };
    const auto vector_64_big{
        integral_to_bytes<std::uint64_t, std::endian::big>( uint_64 )
    };
#ifdef __SIZEOF_INT128__
    const auto vector_128_little{
        integral_to_bytes<__uint128_t, std::endian::little>( uint_128 )
    };
    const auto vector_128_big{ integral_to_bytes<__uint128_t, std::endian::big>(
        uint_128 ) };
#endif

    // Test cases:

    // Direct little -> little & big -> big cases
    // 1 byte uinteger:
    auto test_1_byte_uint_little_cis{
        span_to_integer<std::uint8_t, std::endian::little, std::endian::little>(
            vector_8_little )
        == convert_endian<std::endian::native, std::endian::little>( uint_8 )
    };
    if ( !test_1_byte_uint_little_cis ) {
        std::cerr << "span_to_integer<std::uint8_t, std::endian::little, "
                     "std::endian::little>(vector_8_little) failed."
                  << std::endl;
    }
    auto test_1_byte_uint_big_cis{
        span_to_integer<std::uint8_t, std::endian::big, std::endian::big>(
            vector_8_big )
        == convert_endian<std::endian::native, std::endian::big>( uint_8 )
    };
    if ( !test_1_byte_uint_big_cis ) {
        std::cerr << "span_to_integer<std::uint8_t, std::endian::big, "
                     "std::endian::big>(vector_8_big) failed."
                  << std::endl;
    }
    // 2 byte uinteger:
    auto test_2_byte_uint_little_cis{
        span_to_integer<std::uint16_t, std::endian::little,
                        std::endian::little>( vector_16_little )
        == convert_endian<std::endian::native, std::endian::little>( uint_16 )
    };
    if ( !test_2_byte_uint_little_cis ) {
        std::cerr << "span_to_integer<std::uint16_t, std::endian::little, "
                     "std::endian::little>(vector_16_little) failed."
                  << std::endl;
    }
    auto test_2_byte_uint_big_cis{
        span_to_integer<std::uint16_t, std::endian::big, std::endian::big>(
            vector_16_big )
        == convert_endian<std::endian::native, std::endian::big>( uint_16 )
    };
    if ( !test_2_byte_uint_big_cis ) {
        std::cerr << "span_to_integer<std::uint16_t, std::endian::big, "
                     "std::endian::big>(vector_16_bit) failed."
                  << std::endl;
    }
    // 4 byte uinteger:
    auto test_4_byte_uint_little_cis{
        span_to_integer<std::uint32_t, std::endian::little,
                        std::endian::little>( vector_32_little )
        == convert_endian<std::endian::native, std::endian::little>( uint_32 )
    };
    if ( !test_4_byte_uint_little_cis ) {
        std::cerr << "span_to_integer<std::uint32_t, std::endian::little, "
                     "std::endian::little>(vector_32_little) failed."
                  << std::endl;
    }
    auto test_4_byte_uint_big_cis{
        span_to_integer<std::uint32_t, std::endian::big, std::endian::big>(
            vector_32_big )
        == convert_endian<std::endian::native, std::endian::big>( uint_32 )
    };
    if ( !test_4_byte_uint_big_cis ) {
        std::cerr << "span_to_integer<std::uint32_t, std::endian::big, "
                     "std::endian::big>(vector_32_big) failed."
                  << std::endl;
    }
    // 8 byte uinteger:
    auto test_8_byte_uint_little_cis{
        span_to_integer<std::uint64_t, std::endian::little,
                        std::endian::little>( vector_64_little )
        == convert_endian<std::endian::native, std::endian::little>( uint_64 )
    };
    if ( !test_8_byte_uint_little_cis ) {
        std::cerr << "span_to_integer<std::uint64_t, std::endian::little, "
                     "std::endian::little>(vector_64_little) failed."
                  << std::endl;
    }
    auto test_8_byte_uint_big_cis{
        span_to_integer<std::uint64_t, std::endian::big, std::endian::big>(
            vector_64_big )
        == convert_endian<std::endian::native, std::endian::big>( uint_64 )
    };
    if ( !test_8_byte_uint_big_cis ) {
        std::cerr << "span_to_integer<std::uint64_t, std::endian::big, "
                     "std::endian::big>(vector_64_big) failed."
                  << std::endl;
    }
#ifdef __SIZEOF_INT128__
    // Generic case:
    auto test_large_uint_little_cis{
        span_to_integer<__uint128_t, std::endian::little, std::endian::little>(
            vector_128_little )
        == convert_endian<std::endian::native, std::endian::little>( uint_128 )
    };
    if ( !test_large_uint_little_cis ) {
        std::cerr << "span_to_integer<__uint128_t, std::endian::little, "
                     "std::endian::little>(vector_128_little) failed."
                  << std::endl;
    }
    auto test_large_uint_big_cis{
        span_to_integer<__uint128_t, std::endian::big, std::endian::big>(
            vector_128_big )
        == convert_endian<std::endian::native, std::endian::big>( uint_128 )
    };
    if ( !test_large_uint_big_cis ) {
        std::cerr << "span_to_integer<__128_t, std::endian::big, "
                     "std::endian::big>(vector_128_big) failed."
                  << std::endl;
    }
#endif

    // Cross little -> big & big -> little cases
    // 1 byte uinteger:
    auto test_1_byte_uint_little_trans{
        span_to_integer<std::uint8_t, std::endian::little, std::endian::big>(
            vector_8_little )
        == convert_endian<std::endian::native, std::endian::big>( uint_8 )
    };
    if ( !test_1_byte_uint_little_trans ) {
        std::cerr << "span_to_integer<std::uint8_t, std::endian::little, "
                     "std::endian::big>(vector_8_little) failed."
                  << std::endl;
    }
    auto test_1_byte_uint_big_trans{
        span_to_integer<std::uint8_t, std::endian::big, std::endian::little>(
            vector_8_big )
        == convert_endian<std::endian::native, std::endian::little>( uint_8 )
    };
    if ( !test_1_byte_uint_big_trans ) {
        std::cerr << "span_to_integer<std::uint8_t, std::endian::big, "
                     "std::endian::little>(vector_8_big) failed."
                  << std::endl;
    }
    // 2 byte uinteger:
    auto test_2_byte_uint_little_trans{
        span_to_integer<std::uint16_t, std::endian::little, std::endian::big>(
            vector_16_little )
        == convert_endian<std::endian::native, std::endian::big>( uint_16 )
    };
    if ( !test_2_byte_uint_little_trans ) {
        std::cerr << "span_to_integer<std::uint16_t, std::endian::little, "
                     "std::endian::big>(vector_16_little) failed."
                  << std::endl;
    }
    auto test_2_byte_uint_big_trans{
        span_to_integer<std::uint16_t, std::endian::big, std::endian::little>(
            vector_16_big )
        == convert_endian<std::endian::native, std::endian::little>( uint_16 )
    };
    if ( !test_2_byte_uint_big_trans ) {
        std::cerr << "span_to_integer<std::uint16_t, std::endian::big, "
                     "std::endian::little>(vector_16_bit) failed."
                  << std::endl;
    }
    // 4 byte uinteger:
    auto test_4_byte_uint_little_trans{
        span_to_integer<std::uint32_t, std::endian::little, std::endian::big>(
            vector_32_little )
        == convert_endian<std::endian::native, std::endian::big>( uint_32 )
    };
    if ( !test_4_byte_uint_little_trans ) {
        std::cerr << "span_to_integer<std::uint32_t, std::endian::little, "
                     "std::endian::big>(vector_32_little) failed."
                  << std::endl;
    }
    auto test_4_byte_uint_big_trans{
        span_to_integer<std::uint32_t, std::endian::big, std::endian::little>(
            vector_32_big )
        == convert_endian<std::endian::native, std::endian::little>( uint_32 )
    };
    if ( !test_4_byte_uint_big_trans ) {
        std::cerr << "span_to_integer<std::uint32_t, std::endian::big, "
                     "std::endian::little>(vector_32_big) failed."
                  << std::endl;
    }
    // 8 byte uinteger:
    auto test_8_byte_uint_little_trans{
        span_to_integer<std::uint64_t, std::endian::little, std::endian::big>(
            vector_64_little )
        == convert_endian<std::endian::native, std::endian::big>( uint_64 )
    };
    if ( !test_8_byte_uint_little_trans ) {
        std::cerr << "span_to_integer<std::uint64_t, std::endian::little, "
                     "std::endian::big>(vector_64_little) failed."
                  << std::endl;
    }
    auto test_8_byte_uint_big_trans{
        span_to_integer<std::uint64_t, std::endian::big, std::endian::little>(
            vector_64_big )
        == convert_endian<std::endian::native, std::endian::little>( uint_64 )
    };
    if ( !test_8_byte_uint_big_trans ) {
        std::cerr << "span_to_integer<std::uint64_t, std::endian::big, "
                     "std::endian::little>(vector_64_big) failed."
                  << std::endl;
    }
#ifdef __SIZEOF_INT128__
    // Generic case:
    auto test_large_uint_little_trans{
        span_to_integer<__uint128_t, std::endian::little, std::endian::big>(
            vector_128_little )
        == convert_endian<std::endian::native, std::endian::big>( uint_128 )
    };
    if ( !test_large_uint_little_trans ) {
        std::cerr << "span_to_integer<__uint128_t, std::endian::little, "
                     "std::endian::big>(vector_128_little) failed."
                  << std::endl;
    }
    auto test_large_uint_big_trans{
        span_to_integer<__uint128_t, std::endian::big, std::endian::little>(
            vector_128_big )
        == convert_endian<std::endian::native, std::endian::little>( uint_128 )
    };
    if ( !test_large_uint_big_trans ) {
        std::cerr << "span_to_integer<__128_t, std::endian::big, "
                     "std::endian::little>(vector_128_big) failed."
                  << std::endl;
    }
#endif

    return test_1_byte_uint_little_cis && test_1_byte_uint_big_cis
           && test_2_byte_uint_little_cis && test_2_byte_uint_big_cis
           && test_4_byte_uint_little_cis && test_4_byte_uint_big_cis
           && test_8_byte_uint_little_cis && test_8_byte_uint_big_cis
           && test_large_uint_little_cis && test_large_uint_big_cis
           && test_1_byte_uint_little_trans && test_1_byte_uint_big_trans
           && test_2_byte_uint_little_trans && test_2_byte_uint_big_trans
           && test_4_byte_uint_little_trans && test_4_byte_uint_big_trans
           && test_8_byte_uint_little_trans && test_8_byte_uint_big_trans
           && test_large_uint_little_trans && test_large_uint_big_trans;
}

} // namespace COMMON_TEST

int
common_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t test_passes{ 0 };

    for ( const auto & [name, test] : COMMON_TEST::test_functions ) {
        std::println( "Running {}... ", name );
        const auto result{ test() };
        test_passes += static_cast<std::size_t>( result );
        std::println( "\t{}.", ( result ? "Passed" : "Failed" ) );
    }

    return static_cast<int>( COMMON_TEST::test_functions.size() - test_passes );
}
