#pragma once

#include <bit>

#define NOMOVE( class )         \
    class( class && ) = delete; \
    class & operator=( class && ) = delete;

#define NOCOPY( class )              \
    class( const class & ) = delete; \
    class & operator=( const class & ) = delete;

#define COLD [[unlikely]]

template <std::integral T, std::endian E = std::endian::native>
[[nodiscard]] constexpr T
byteswap( const T value ) {
    if constexpr ( E == std::endian::little ) {
        return std::byteswap( value );
    }
    else {
        return value;
    }
}
