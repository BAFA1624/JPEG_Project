#pragma once

#include "common/common.hpp"

#include <array>
#include <functional>
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
constexpr std::vector<std::byte>
integral_to_bytes( const T integer ) {
    static_assert( sizeof( T ) < std::numeric_limits<T>::max()
                   && sizeof( T ) < std::numeric_limits<std::size_t>::max() );

    std::vector<std::byte> bytes( sizeof( T ) );

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
    constexpr const auto  endian = std::endian::native;
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

constexpr bool test_convert_endian();
constexpr bool test_lsb_msb();
constexpr bool test_lsB_msB();
constexpr bool test_lsb_msb_offset_consteval();
constexpr bool test_lsB_msB_offset_consteval();
constexpr bool test_lsb_msb_offset_constexpr();
constexpr bool test_lsB_msB_offset_constexpr();
bool           test_span_to_integer();

const auto test_functions =
    std::vector<std::tuple<std::string_view, std::function<bool()>>>{
        { "test_convert_endian", test_convert_endian },
        { "test_lsb_msb", test_lsb_msb },
        { "test_lsB_msB", test_lsB_msB },
        { "test_lsb_msb_offset_consteval", test_lsb_msb_offset_consteval },
        { "test_lsB_msB_offset_consteval", test_lsB_msB_offset_consteval },
        { "test_lsb_msb_offset_constexpr", test_lsb_msb_offset_constexpr },
        { "test_lsB_msB_offset_constexpr", test_lsB_msB_offset_constexpr },
        { "test_span_to_integer", test_span_to_integer }
    };

} // namespace COMMON_TEST

// Required by CTest
int common_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv );
