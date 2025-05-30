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

constexpr inline std::size_t byte_bits{ 8 };

constexpr inline std::ostream &
operator<<( std::ostream & out_stream, const std::byte byte ) {
    return out_stream << std::bitset<8>( std::to_integer<unsigned>( byte ) );
}

template <std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native, std::integral T>
[[nodiscard]] constexpr inline T
convert_endian( const T value ) {
    if constexpr ( SourceEndian != TargetEndian ) {
        return std::byteswap( value );
    }
    else {
        return value;
    }
}

// clang-format off
template <std::endian baseEndianness, std::size_t N = 32>
constexpr inline std::size_t lsb{ (
    baseEndianness == std::endian::little ? 0 :
    baseEndianness == std::endian::big    ? N - 1 :
                                            N - 1 ) };
template <std::endian baseEndianness, std::size_t N = 32>
constexpr inline std::size_t msb{ (
    baseEndianness == std::endian::little ? N - 1 :
    baseEndianness == std::endian::big    ? 0 :
                                            0 ) };
// clang-format on
