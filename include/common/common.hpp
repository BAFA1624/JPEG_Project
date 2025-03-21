#pragma once

#include <bit>
#include <bitset>
#include <ostream>

#define NOMOVE( class )         \
    class( class && ) = delete; \
    class & operator=( class && ) = delete;

#define NOCOPY( class )              \
    class( const class & ) = delete; \
    class & operator=( const class & ) = delete;

#define COLD [[unlikely]]

constexpr inline std::ostream &
operator<<( std::ostream & out_stream, const std::byte byte ) {
    return out_stream << std::bitset<8>( std::to_integer<unsigned>( byte ) );
}

template <std::integral T, std::endian E = std::endian::native>
[[nodiscard]] constexpr inline T
byteswap( const T value ) {
    if constexpr ( E == std::endian::little ) {
        return std::byteswap( value );
    }
    else {
        return value;
    }
}
