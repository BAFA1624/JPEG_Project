#include "common/common_test.hpp"

#include <algorithm>
#include <format>
#include <random>

namespace COMMON_TEST
{

bool
test_convert_endian() {
    constexpr auto little_little = []( const auto value ) {
        return convert_endian<std::endian::little, std::endian::little>(
            value );
    };
    constexpr auto little_big = []( const auto value ) {
        return convert_endian<std::endian::little, std::endian::big>( value );
    };
    constexpr auto big_little = []( const auto value ) {
        return convert_endian<std::endian::big, std::endian::little>( value );
    };
    constexpr auto big_big = []( const auto value ) {
        return convert_endian<std::endian::big, std::endian::big>( value );
    };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            little_little, 0x12345678u, 0x12345678u ),
        TEST_INTERFACE::test_function( little_big, 0x12345678u, 0x78563412u ),
        TEST_INTERFACE::test_function( big_little, 0x78563412u, 0x12345678u ),
        TEST_INTERFACE::test_function( big_big, 0x78563412u, 0x78563412u )
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_lsb_msb() {
    constexpr std::uint32_t size{ sizeof( size ) * byte_bits };

    constexpr auto little_lsb = [size]() {
        return lsb<decltype( size ), std::endian::little>;
    };
    constexpr auto little_msb = [size]() {
        return msb<decltype( size ), std::endian::little>;
    };
    constexpr auto big_lsb = [size]() {
        return lsb<decltype( size ), std::endian::big>;
    };
    constexpr auto big_msb = [size]() {
        return msb<decltype( size ), std::endian::big>;
    };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function( little_lsb, std::uint32_t{ 0 } ),
        TEST_INTERFACE::test_function( little_msb, std::uint32_t{ 31 } ),
        TEST_INTERFACE::test_function( big_lsb, std::uint32_t{ 31 } ),
        TEST_INTERFACE::test_function( big_msb, std::uint32_t{ 0 } )
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_lsB_msB() {
    constexpr std::uint32_t size{ sizeof( size ) };

    constexpr auto little_lsB = [size]() {
        return lsB<decltype( size ), std::endian::little>;
    };
    constexpr auto little_msB = [size]() {
        return msB<decltype( size ), std::endian::little>;
    };
    constexpr auto big_lsB = [size]() {
        return lsB<decltype( size ), std::endian::big>;
    };
    constexpr auto big_msB = [size]() {
        return msB<decltype( size ), std::endian::big>;
    };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function( little_lsB, std::uint32_t{ 0 } ),
        TEST_INTERFACE::test_function( little_msB, std::uint32_t{ 3 } ),
        TEST_INTERFACE::test_function( big_lsB, std::uint32_t{ 3 } ),
        TEST_INTERFACE::test_function( big_msB, std::uint32_t{ 0 } )
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_lsb_msb_offset_constexpr() {
    using T = std::uint32_t;
    constexpr T size{ byte_bits * sizeof( T ) };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            []() { return lsb_offset<std::endian::little, T, 0>(); }, T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return lsb_offset<std::endian::little, T, 31>(); },
            T{ size - 1 } ),
        TEST_INTERFACE::test_function(
            []() { return msb_offset<std::endian::little, T, 0>(); },
            T{ size - 1 } ),
        TEST_INTERFACE::test_function(
            []() { return msb_offset<std::endian::little, T, 31>(); }, T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return lsb_offset<std::endian::big, T, 0>(); },
            T{ size - 1 } ),
        TEST_INTERFACE::test_function(
            []() { return lsb_offset<std::endian::big, T, 31>(); }, T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return msb_offset<std::endian::big, T, 0>(); }, T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return msb_offset<std::endian::big, T, 31>(); },
            T{ size - 1 } )

    };

    return TEST_INTERFACE::confirm_results( test_results );
}


bool
test_lsB_msB_offset_constexpr() {
    using T = std::uint32_t;
    constexpr T size{ sizeof( T ) };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            []() { return lsB_offset<std::endian::little, T, 0>(); }, T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return lsB_offset<std::endian::little, T, size - 1>(); },
            T{ size - 1 } ),
        TEST_INTERFACE::test_function(
            []() { return msB_offset<std::endian::little, T, 0>(); },
            T{ size - 1 } ),
        TEST_INTERFACE::test_function(
            []() { return msB_offset<std::endian::little, T, size - 1>(); },
            T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return lsB_offset<std::endian::big, T, 0>(); },
            T{ size - 1 } ),
        TEST_INTERFACE::test_function(
            []() { return lsB_offset<std::endian::big, T, size - 1>(); },
            T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return msB_offset<std::endian::big, T, 0>(); }, T{ 0 } ),
        TEST_INTERFACE::test_function(
            []() { return msB_offset<std::endian::big, T, size - 1>(); },
            T{ size - 1 } ),
    };

    return TEST_INTERFACE::confirm_results( test_results );
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

    const auto test_results = std::vector<bool>{
        // little -> little & big -> big tests.
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint8_t,
                                       std::endian::little,
                                       std::endian::little>( vector_8_little );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_8 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint8_t,
                                       std::endian::big,
                                       std::endian::big>( vector_8_big );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_8 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint16_t,
                                       std::endian::little,
                                       std::endian::little>( vector_16_little );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_16 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint16_t,
                                       std::endian::big,
                                       std::endian::big>( vector_16_big );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_16 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint32_t,
                                       std::endian::little,
                                       std::endian::little>( vector_32_little );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_32 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint32_t,
                                       std::endian::big,
                                       std::endian::big>( vector_32_big );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_32 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint32_t,
                                       std::endian::little,
                                       std::endian::little>( vector_32_little );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_32 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint32_t,
                                       std::endian::big,
                                       std::endian::big>( vector_32_big );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_32 ) ),
#ifdef __SIZEOF_INT128__
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<__uint128_t,
                                       std::endian::little,
                                       std::endian::little>(
                    vector_128_little );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_128 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<__uint128_t,
                                       std::endian::big,
                                       std::endian::big>( vector_128_big );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_128 ) ),
#endif
        // little -> big & big -> little tests.
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint8_t,
                                       std::endian::little,
                                       std::endian::big>( vector_8_little );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_8 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint8_t,
                                       std::endian::big,
                                       std::endian::little>( vector_8_big );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_8 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint16_t,
                                       std::endian::little,
                                       std::endian::big>( vector_16_little );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_16 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint16_t,
                                       std::endian::big,
                                       std::endian::little>( vector_16_big );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_16 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint32_t,
                                       std::endian::little,
                                       std::endian::big>( vector_32_little );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_32 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint32_t,
                                       std::endian::big,
                                       std::endian::little>( vector_32_big );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_32 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint64_t,
                                       std::endian::little,
                                       std::endian::big>( vector_64_little );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_64 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<std::uint64_t,
                                       std::endian::big,
                                       std::endian::little>( vector_64_big );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_64 ) ),
#ifdef __SIZEOF_INT128__
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<__uint128_t,
                                       std::endian::little,
                                       std::endian::big>( vector_128_little );
            },
            convert_endian<std::endian::native, std::endian::big>( uint_128 ) ),
        TEST_INTERFACE::test_function(
            [&]() {
                return span_to_integer<__uint128_t,
                                       std::endian::big,
                                       std::endian::little>( vector_128_big );
            },
            convert_endian<std::endian::native, std::endian::little>(
                uint_128 ) )
#endif
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_get_byte() {
    constexpr std::uint32_t   var{ 0b10000000'01000000'00100000'00010000 };
    constexpr std::bitset<32> test{ var };
    constexpr std::uint32_t   var_big{
        convert_endian<std::endian::native, std::endian::big>( var )
    };
    constexpr std::uint32_t var_little{
        convert_endian<std::endian::native, std::endian::little>( var )
    };

    constexpr auto answers_1 =
        std::array<std::byte, 4>{ std::byte{ 0b10000000 },
                                  std::byte{ 0b01000000 },
                                  std::byte{ 0b00100000 },
                                  std::byte{ 0b00010000 } };
    constexpr auto answers_2 =
        std::array<std::byte, 4>{ std::byte{ 0b00010000 },
                                  std::byte{ 0b00100000 },
                                  std::byte{ 0b01000000 },
                                  std::byte{ 0b10000000 } };

    constexpr auto big_answers =
        std::endian::native == std::endian::little ? answers_1 : answers_2;
    constexpr auto little_answers =
        std::endian::native == std::endian::little ? answers_2 : answers_1;

    [[maybe_unused]] constexpr auto print_bytes =
        []( const std::bitset<32> & bits ) {
            std::cout << bits << "\n";
            const std::bitset<8> byte_0 =
                static_cast<std::uint8_t>( ( bits >> 24 ).to_ulong() );
            const std::bitset<8> byte_1 =
                static_cast<std::uint8_t>( ( bits >> 16 ).to_ulong() );
            const std::bitset<8> byte_2 =
                static_cast<std::uint8_t>( ( bits >> 8 ).to_ulong() );
            const std::bitset<8> byte_3 =
                static_cast<std::uint8_t>( ( bits >> 0 ).to_ulong() );
            std::cout << "0 | " << byte_0 << "\n";
            std::cout << "1 | " << byte_1 << "\n";
            std::cout << "2 | " << byte_2 << "\n";
            std::cout << "3 | " << byte_3 << "\n";
        };

    constexpr auto byte_0 = []( const auto value ) {
        return get_byte<0>( value );
    };
    constexpr auto byte_1 = []( const auto value ) {
        return get_byte<1>( value );
    };
    constexpr auto byte_2 = []( const auto value ) {
        return get_byte<2>( value );
    };
    constexpr auto byte_3 = []( const auto value ) {
        return get_byte<3>( value );
    };
    const auto byte = []( const auto value, const auto idx ) {
        return get_byte( value, idx );
    };
    [[maybe_unused]] const auto byte_0_print = [&print_bytes,
                                                &byte_0]( const auto value ) {
        print_bytes( value );
        std::cout << " [" << std::dec << 0 << "] --> " << std::hex
                  << std::bitset<8>{ std::to_integer<std::uint8_t>(
                         byte_0( value ) ) }
                  << std::dec << "\n";
        return get_byte( value, 0 );
    };
    [[maybe_unused]] const auto byte_1_print = [&print_bytes,
                                                &byte_1]( const auto value ) {
        print_bytes( value );
        std::cout << " [" << std::dec << 1 << "] --> " << std::hex
                  << std::bitset<8>{ std::to_integer<std::uint8_t>(
                         byte_1( value ) ) }
                  << std::dec << "\n";
        return get_byte( value, 1 );
    };
    [[maybe_unused]] const auto byte_2_print = [&print_bytes,
                                                &byte_2]( const auto value ) {
        print_bytes( value );
        std::cout << " [" << std::dec << 2 << "] --> " << std::hex
                  << std::bitset<8>{ std::to_integer<std::uint8_t>(
                         byte_2( value ) ) }
                  << std::dec << "\n";
        return get_byte( value, 2 );
    };
    [[maybe_unused]] const auto byte_3_print = [&print_bytes,
                                                &byte_3]( const auto value ) {
        print_bytes( value );
        std::cout << " [" << std::dec << 3 << "] --> " << std::hex
                  << std::bitset<8>{ std::to_integer<std::uint8_t>(
                         byte_3( value ) ) }
                  << std::dec << "\n";
        return get_byte( value, 3 );
    };
    [[maybe_unused]] const auto byte_print = [&print_bytes]( const auto value,
                                                             const auto idx ) {
        print_bytes( value );
        std::cout << " [" << std::dec << idx << "] --> " << std::hex
                  << std::bitset<8>{ std::to_integer<std::uint8_t>(
                         get_byte( value, idx ) ) }
                  << std::dec << "\n";
        return get_byte( value, idx );
    };


    std::cout << "Native endian: "
              << ( std::endian::native == std::endian::big ? "Big" : "Little" )
              << std::endl;

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            byte_0, big_answers[0], var_big ), // Fail
        TEST_INTERFACE::test_function( byte_1, big_answers[1], var_big ),
        TEST_INTERFACE::test_function( byte_2, big_answers[2], var_big ),
        TEST_INTERFACE::test_function( byte_3, big_answers[3], var_big ),
        // ---
        TEST_INTERFACE::test_function(
            byte, big_answers[0], var_big, std::uint32_t{ 0 } ), // Fail
        TEST_INTERFACE::test_function(
            byte, big_answers[1], var_big, std::uint32_t{ 1 } ),
        TEST_INTERFACE::test_function(
            byte, big_answers[2], var_big, std::uint32_t{ 2 } ),
        TEST_INTERFACE::test_function(
            byte, big_answers[3], var_big, std::uint32_t{ 3 } ),
        // ---
        TEST_INTERFACE::test_function( byte_0, little_answers[0], var_little ),
        TEST_INTERFACE::test_function( byte_1, little_answers[1], var_little ),
        TEST_INTERFACE::test_function( byte_2, little_answers[2], var_little ),
        TEST_INTERFACE::test_function( byte_3, little_answers[3], var_little ),
        // ---
        TEST_INTERFACE::test_function(
            byte, little_answers[0], var_little, std::uint32_t{ 0 } ),
        TEST_INTERFACE::test_function(
            byte, little_answers[1], var_little, std::uint32_t{ 1 } ),
        TEST_INTERFACE::test_function(
            byte, little_answers[2], var_little, std::uint32_t{ 2 } ),
        TEST_INTERFACE::test_function(
            byte, little_answers[3], var_little, std::uint32_t{ 3 } )
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_to_bytes() {
    // Test sources
    constexpr std::uint8_t  uint_8{ 0xEF };
    constexpr std::uint16_t uint_16{ 0x12'34 };
    constexpr std::uint32_t uint_32{ 0x12'34'56'78 };
    constexpr std::uint64_t uint_64{ 0x12'34'56'78'90'AB'CD'EF };
#ifdef __SIZEOF_INT128__
    constexpr auto construct_uint128_t = []( const std::uint64_t hi,
                                             const std::uint64_t lo ) {
        return ( static_cast<__uint128_t>( hi ) << 64 )
               | static_cast<__uint128_t>( lo );
    };
    constexpr __uint128_t uint_128{ construct_uint128_t(
        0x12'34'56'78'90'AB'CD'EF, 0x12'34'56'78'90'AB'CD'EF ) };

#endif

    constexpr auto uint_8_big{
        convert_endian<std::endian::native, std::endian::big>( uint_8 )
    };
    constexpr auto uint_16_big{
        convert_endian<std::endian::native, std::endian::big>( uint_16 )
    };
    constexpr auto uint_32_big{
        convert_endian<std::endian::native, std::endian::big>( uint_32 )
    };
    constexpr auto uint_64_big{
        convert_endian<std::endian::native, std::endian::big>( uint_64 )
    };
    constexpr auto uint_8_little{
        convert_endian<std::endian::native, std::endian::little>( uint_8 )
    };
    constexpr auto uint_16_little{
        convert_endian<std::endian::native, std::endian::little>( uint_16 )
    };
    constexpr auto uint_32_little{
        convert_endian<std::endian::native, std::endian::little>( uint_32 )
    };
    constexpr auto uint_64_little{
        convert_endian<std::endian::native, std::endian::little>( uint_64 )
    };
#ifdef __SIZEOF_INT128__
    [[maybe_unused]] constexpr auto uint_128_big{
        convert_endian<std::endian::native, std::endian::big>( uint_128 )
    };
    [[maybe_unused]] constexpr auto uint_128_little{
        convert_endian<std::endian::native, std::endian::little>( uint_128 )
    };
#endif

    [[maybe_unused]] const auto print = []( const auto             value,
                                            const std::string_view name ) {
        std::cout << name << ": " << std::hex
                  << static_cast<std::uint64_t>( value ) << " --> "
                  << std::bit_cast<std::array<std::byte, sizeof( value )>>(
                         value )
                  << std::dec << "\n";
    };

    /*  std::println( "Underlying representation (Hex):" );
        print( uint_8, "uint_8" );
        print( uint_8_big, "uint_8_big" );
        print( uint_8_little, "uint_8_little" );
        print( uint_16, "uint_16" );
        print( uint_16_big, "uint_16_big" );
        print( uint_16_little, "uint_16_little" );
        print( uint_32, "uint_32" );
        print( uint_32_big, "uint_32_big" );
        print( uint_32_little, "uint_32_little" );
        print( uint_64, "uint_64" );
        print( uint_64_big, "uint_64_big" );
        print( uint_64_little, "uint_64_little" );
    #ifdef __SIZEOF_INT128__
        print( uint_128, "uint_128" );
        print( uint_128_big, "uint_128_big" );
        print( uint_128_little, "uint_128_little" );
    #endif
    */

    // Example Solutions - All declared assuming big endian
    constexpr auto result_8 = std::array<std::byte, 1>{ std::byte{ 0xEF } };
    constexpr auto result_16 =
        std::array<std::byte, 2>{ std::byte{ 0x12 }, std::byte{ 0x34 } };
    constexpr auto result_32 = std::array<std::byte, 4>{ std::byte{ 0x12 },
                                                         std::byte{ 0x34 },
                                                         std::byte{ 0x56 },
                                                         std::byte{ 0x78 } };
    constexpr auto result_64 =
        std::array<std::byte, 8>{ std::byte{ 0x12 }, std::byte{ 0x34 },
                                  std::byte{ 0x56 }, std::byte{ 0x78 },
                                  std::byte{ 0x90 }, std::byte{ 0xAB },
                                  std::byte{ 0xCD }, std::byte{ 0xEF } };
#ifdef __SIZEOF_INT128__
    [[maybe_unused]] constexpr auto result_128 = std::array<std::byte, 16>{
        std::byte{ 0x12 }, std::byte{ 0x34 }, std::byte{ 0x56 },
        std::byte{ 0x78 }, std::byte{ 0x90 }, std::byte{ 0xAB },
        std::byte{ 0xCD }, std::byte{ 0xEF }, std::byte{ 0x12 },
        std::byte{ 0x34 }, std::byte{ 0x56 }, std::byte{ 0x78 },
        std::byte{ 0x90 }, std::byte{ 0xAB }, std::byte{ 0xCD },
        std::byte{ 0xEF }
    };
#endif

    constexpr std::array<std::byte, 1> big_8_result{ result_8 };
    std::array<std::byte, 2>           big_16_result{};
    std::array<std::byte, 4>           big_32_result{};
    std::array<std::byte, 8>           big_64_result{};
    constexpr std::array<std::byte, 1> little_8_result{ result_8 };
    std::array<std::byte, 2>           little_16_result{};
    std::array<std::byte, 4>           little_32_result{};
    std::array<std::byte, 8>           little_64_result{};
#ifdef __SIZEOF_INT128__
    [[maybe_unused]] std::array<std::byte, 16> big_128_result{};
    [[maybe_unused]] std::array<std::byte, 16> little_128_result{};
#endif

    if constexpr ( std::endian::native == std::endian::little ) {
        big_16_result = result_16;
        big_32_result = result_32;
        big_64_result = result_64;

        std::ranges::reverse_copy(
            result_16.cbegin(), result_16.cend(), little_16_result.begin() );
        std::ranges::reverse_copy(
            result_32.cbegin(), result_32.cend(), little_32_result.begin() );
        std::ranges::reverse_copy(
            result_64.cbegin(), result_64.cend(), little_64_result.begin() );

#ifdef __SIZEOF_INT128__
        big_128_result = result_128;
        std::ranges::reverse_copy(
            result_128.cbegin(), result_128.cend(), little_128_result.begin() );
#endif
    }
    else {
        little_16_result = result_16;
        little_32_result = result_32;
        little_64_result = result_64;

        std::ranges::reverse_copy(
            result_16.cbegin(), result_16.cend(), big_16_result.begin() );
        std::ranges::reverse_copy(
            result_32.cbegin(), result_32.cend(), big_32_result.begin() );
        std::ranges::reverse_copy(
            result_64.cbegin(), result_64.cend(), big_64_result.begin() );

#ifdef __SIZEOF_INT128__
        little_128_result = result_128;
        std::ranges::reverse_copy(
            result_128.cbegin(), result_128.cend(), big_128_result.begin() );
#endif
    }

    [[maybe_unused]] constexpr auto print_val = []( const auto & value ) {
        std::cout << std::hex << value << std::dec << "\n";
    };

    // Test Functions
    const auto test_little_little_8 = [&]( const std::uint8_t value ) {
        const auto result =
            to_bytes<std::uint8_t, std::endian::little, std::endian::little>(
                value );
        return result;
    };
    const auto test_big_big_8 = [&]( const std::uint8_t value ) {
        const auto result =
            to_bytes<std::uint8_t, std::endian::big, std::endian::big>( value );
        return result;
    };
    const auto test_little_little_16 = [&]( const std::uint16_t value ) {
        const auto result =
            to_bytes<std::uint16_t, std::endian::little, std::endian::little>(
                value );
        return result;
    };
    const auto test_big_big_16 = [&]( const std::uint16_t value ) {
        const auto result =
            to_bytes<std::uint16_t, std::endian::big, std::endian::big>(
                value );
        return result;
    };
    const auto test_little_little_32 = [&]( const std::uint32_t value ) {
        const auto result =
            to_bytes<std::uint32_t, std::endian::little, std::endian::little>(
                value );
        return result;
    };
    const auto test_big_big_32 = [&]( const std::uint32_t value ) {
        const auto result =
            to_bytes<std::uint32_t, std::endian::big, std::endian::big>(
                value );
        return result;
    };
    const auto test_little_little_64 = [&]( const std::uint64_t value ) {
        const auto result =
            to_bytes<std::uint64_t, std::endian::little, std::endian::little>(
                value );
        return result;
    };
    const auto test_big_big_64 = [&]( const std::uint64_t value ) {
        const auto result =
            to_bytes<std::uint64_t, std::endian::big, std::endian::big>(
                value );
        return result;
    };
#ifdef __SIZEOF_INT128__
    [[maybe_unused]] const auto test_little_little_128 =
        [&]( const __uint128_t value ) {
            const auto result =
                to_bytes<__uint128_t, std::endian::little, std::endian::little>(
                    value );
            return result;
        };
    [[maybe_unused]] const auto test_big_big_128 =
        [&]( const __uint128_t value ) {
            const auto result =
                to_bytes<__uint128_t, std::endian::big, std::endian::big>(
                    value );
            return result;
        };
#endif

    const auto test_little_big_8 = [&]( const std::uint8_t value ) {
        const auto result =
            to_bytes<std::uint8_t, std::endian::little, std::endian::big>(
                value );
        return result;
    };
    const auto test_big_little_8 = [&]( const std::uint8_t value ) {
        const auto result =
            to_bytes<std::uint8_t, std::endian::big, std::endian::little>(
                value );
        return result;
    };
    const auto test_little_big_16 = [&]( const std::uint16_t value ) {
        const auto result =
            to_bytes<std::uint16_t, std::endian::little, std::endian::big>(
                value );
        return result;
    };
    const auto test_big_little_16 = [&]( const std::uint16_t value ) {
        const auto result =
            to_bytes<std::uint16_t, std::endian::big, std::endian::little>(
                value );
        return result;
    };
    const auto test_little_big_32 = [&]( const std::uint32_t value ) {
        const auto result =
            to_bytes<std::uint32_t, std::endian::big, std::endian::little>(
                value );
        return result;
    };
    const auto test_big_little_32 = [&]( const std::uint32_t value ) {
        const auto result =
            to_bytes<std::uint32_t, std::endian::big, std::endian::little>(
                value );
        return result;
    };
    const auto test_little_big_64 = [&]( const std::uint64_t value ) {
        const auto result =
            to_bytes<std::uint64_t, std::endian::little, std::endian::big>(
                value );
        return result;
    };
    const auto test_big_little_64 = [&]( const std::uint64_t value ) {
        const auto result = to_bytes<std::uint64_t,
                                     std::endian::big,
                                     std::endian::little,
                                     false>( value );
        return result;
    };
#ifdef __SIZEOF_INT128__
    [[maybe_unused]] const auto test_little_big_128 =
        [&]( const __uint128_t value ) {
            const auto result =
                to_bytes<__uint128_t, std::endian::little, std::endian::big>(
                    value );
            return result;
        };
    [[maybe_unused]] const auto test_big_little_128 =
        [&]( const __uint128_t value ) {
            const auto result =
                to_bytes<__uint128_t, std::endian::big, std::endian::little>(
                    value );
            return result;
        };
#endif

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_little_little_8, little_8_result, uint_8_little ),
        TEST_INTERFACE::test_function(
            test_little_little_16, little_16_result, uint_16_little ),
        TEST_INTERFACE::test_function(
            test_little_little_32, little_32_result, uint_32_little ),
        TEST_INTERFACE::test_function(
            test_little_little_64, little_64_result, uint_64_little ),
#ifdef __SIZEOF_INT128__
        TEST_INTERFACE::test_function(
            test_little_little_128, little_128_result, uint_128_little ),
#endif
        TEST_INTERFACE::test_function(
            test_big_big_8, big_8_result, uint_8_big ),
        TEST_INTERFACE::test_function(
            test_big_big_16, big_16_result, uint_16_big ),
        TEST_INTERFACE::test_function(
            test_big_big_32, big_32_result, uint_32_big ),
        TEST_INTERFACE::test_function(
            test_big_big_64, big_64_result, uint_64_big ),
#ifdef __SIZEOF_INT128__
        TEST_INTERFACE::test_function(
            test_big_big_128, big_128_result, uint_128_big ),
#endif
        TEST_INTERFACE::test_function(
            test_little_big_8, big_8_result, uint_8_little ),
        TEST_INTERFACE::test_function(
            test_little_big_16, big_16_result, uint_16_little ),
        TEST_INTERFACE::test_function(
            test_little_big_32, big_32_result, uint_32_little ),
        TEST_INTERFACE::test_function(
            test_little_big_64, big_64_result, uint_64_little ),
#ifdef __SIZEOF_INT128__
        TEST_INTERFACE::test_function(
            test_little_big_128, big_128_result, uint_128_little ),
#endif
        TEST_INTERFACE::test_function(
            test_big_little_8, little_8_result, uint_8_big ),
        TEST_INTERFACE::test_function(
            test_big_little_16, little_16_result, uint_16_big ),
        TEST_INTERFACE::test_function(
            test_big_little_32, little_32_result, uint_32_big ),
        TEST_INTERFACE::test_function(
            test_big_little_64, little_64_result, uint_64_big ),
#ifdef __SIZEOF_INT128__
        TEST_INTERFACE::test_function(
            test_big_little_128, little_128_result, uint_128_big ),
#endif
    };

    return TEST_INTERFACE::confirm_results( test_results );
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
