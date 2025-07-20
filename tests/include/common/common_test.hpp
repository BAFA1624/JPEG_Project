#pragma once

#include "common/common.hpp"

#include <limits>
#include <vector>

template <auto T, std::endian E>
    requires std::is_integral_v<decltype( T )>
             && std::is_unsigned_v<decltype( T )>
constexpr std::vector<std::byte>
integral_to_bytes() {
    static_assert( sizeof( T ) < std::numeric_limits<std::size_t>::max() );

    constexpr const std::size_t N{ sizeof( T ) };
    std::vector<std::byte>      bytes( N );

    constexpr const auto byte_index = []( const auto x, const auto i ) {
        return ( x >> ( byte_bits * i ) ) & 0xFF;
    };

    for ( std::size_t i{ 0 }; i < sizeof( T ); ++i ) {
        bytes[msb_offset<E, i, sizeof( T )>()] =
            static_cast<std::byte>( byte_index(
                T, msb_offset<std::endian::native, i, sizeof( T )>() ) );
    }

    return bytes;
}

// template <auto T, std::endian E>
//     requires std::is_integral_v<decltype( T )>
//              && std::is_unsigned_v<decltype( T )>

template <typename T, std::endian E>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
constexpr std::vector<std::byte>
integral_to_bytes( const T x ) {
    static_assert( sizeof( T ) < std::numeric_limits<std::size_t>::max() );

    constexpr const std::size_t N{ sizeof( T ) };
    std::vector<std::byte>      bytes( N );

    constexpr const auto byte_index = []( const auto x, const auto i ) {
        return ( x >> ( byte_bits * i ) ) & 0xFF;
    };

    for ( std::size_t i{ 0 }; i < sizeof( T ); ++i ) {
        bytes[msb_offset<E, i, sizeof( T )>()] =
            static_cast<std::byte>( byte_index(
                x, msb_offset<std::endian::native, i, sizeof( T )>() ) );
    }

    return bytes;
}
