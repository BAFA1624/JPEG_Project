#pragma once

#include "common/common.hpp"
#include "common/test_interface.hpp"

#include <array>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <print>
#include <vector>

namespace COMMON_TEST
{

template <auto T, std::endian E>
    requires std::is_integral_v<decltype( T )>
             && std::is_unsigned_v<decltype( T )>
constexpr std::array<std::byte, sizeof( T )>
integral_to_bytes() {
    static_assert( sizeof( T ) < std::numeric_limits<std::size_t>::max() );

    std::array<std::byte, sizeof( T )> bytes{};

    constexpr const auto byte_index = []( const auto integer, const auto idx ) {
        return ( integer >> ( byte_bits * idx ) ) & 0xFF;
    };

    for ( std::size_t i{ 0 }; i < sizeof( T ); ++i ) {
        bytes[msb_offset<E, sizeof( T )>( i )] =
            static_cast<std::byte>( byte_index(
                T, msb_offset<std::endian::native, sizeof( T )>( i ) ) );
    }

    return bytes;
}

// template <auto T, std::endian E>
//     requires std::is_integral_v<decltype( T )>
//              && std::is_unsigned_v<decltype( T )>

template <typename T, std::endian E>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
constexpr std::array<std::byte, sizeof( T )>
integral_to_bytes( const T integer ) {
    static_assert( sizeof( T ) < std::numeric_limits<T>::max()
                   && sizeof( T ) < std::numeric_limits<std::size_t>::max() );

    std::array<std::byte, sizeof( T )> bytes{};

    constexpr const auto byte_index = []( const auto integer, const auto idx ) {
        return ( integer >> ( byte_bits * idx ) ) & ~0x00;
    };

    for ( T i{ 0 }; i < sizeof( T ); ++i ) {
        bytes[static_cast<std::size_t>( msB_offset<E, T>( i ) )] =
            static_cast<std::byte>( byte_index(
                integer, msB_offset<std::endian::native, T>( i ) ) );
    }

    return bytes;
}

template <typename T>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
void
print_bits( const T value ) {
    constexpr std::size_t byte_count = sizeof( T );

    constexpr auto print_byte = []( const std::uint8_t byte ) {
        for ( int i{ 7 }; i >= 0; --i ) { std::cout << ( ( byte >> i ) & 1 ); }
    };

    const uint8_t * bytes = std::bit_cast<uint8_t *>( &value );

    for ( std::size_t i = 0; i < byte_count; ++i ) {
        print_byte( bytes[i] );
        std::cout << " ";
    }

    std::cout << '\n';
}

// Tests:

bool test_convert_endian();
bool test_lsb_msb();
bool test_lsB_msB();
bool test_lsb_msb_offset_constexpr();
bool test_lsB_msB_offset_constexpr();
bool test_span_to_integer();
bool test_get_byte();
bool test_to_bytes();

const auto test_functions =
    std::vector<std::tuple<std::string_view, std::function<bool()>>>{
        { "test_convert_endian", test_convert_endian },
        { "test_lsb_msb", test_lsb_msb },
        { "test_lsB_msB", test_lsB_msB },
        { "test_lsb_msb_offset_constexpr", test_lsb_msb_offset_constexpr },
        { "test_lsB_msB_offset_constexpr", test_lsB_msB_offset_constexpr },
        { "test_span_to_integer", test_span_to_integer },
        { "test_get_byte", test_get_byte },
        { "test_to_bytes", test_to_bytes }
    };

} // namespace COMMON_TEST

#ifdef __SIZEOF_INT128__
constexpr std::ostream &
operator<<( std::ostream & out, const __uint128_t value ) {
    std::ostream::sentry s( out );

    unsigned     base{ 10 };
    const char * digits{ "0123456789" };

    const auto basefield{ out.flags() & std::ios_base::basefield };
    if ( basefield == std::ios_base::hex ) {
        base = 16;
        digits = "0123456789ABCDEF";
    }
    else if ( basefield == std::ios_base::oct ) {
        base = 8;
        digits = "01234567";
    }

    if ( s ) {
        __uint128_t tmp = value;

        char   buffer[128];
        char * d = std::end( buffer );

        do {
            --d;
            *d = digits[tmp % base];
            tmp /= base;
        } while ( tmp != 0 );

        const auto len{ std::end( buffer ) - d };
        if ( out.rdbuf()->sputn( d, len ) != len ) {
            out.setstate( std::ios_base::badbit );
        }
    }
    return out;
}
#endif

// Required by CTest
int common_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv );
