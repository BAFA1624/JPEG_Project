#pragma once

#include "common/common.hpp"

#include <array>
#include <iostream>
#include <limits>
#include <print>
#include <vector>

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

int common_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv );
