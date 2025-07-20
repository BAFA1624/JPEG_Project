#pragma once

#include <bit>
#include <bitset>
#include <cassert>
#include <ostream>
#include <span>

#define NOMOVE( class )         \
    class( class && ) = delete; \
    class & operator=( class && ) = delete;

#define NOCOPY( class )              \
    class( const class & ) = delete; \
    class & operator=( const class & ) = delete;

#define VIRTUAL_BASE_CLASS( class ) \
    class() = default;              \
    NOCOPY( class )                 \
    NOMOVE( class )                 \
    ~class() = default;

#define COLD [[unlikely]]

constexpr inline std::size_t byte_bits{ 8 };

constexpr std::ostream &
operator<<( std::ostream & out_stream, const std::byte byte ) {
    return out_stream << std::bitset<byte_bits>(
               std::to_integer<unsigned>( byte ) );
}

template <std::endian E>
concept ValidEndian = E == std::endian::little || E == std::endian::big;

template <std::endian sourceEndian,
          std::endian targetEndian = std::endian::native, std::integral T>
[[nodiscard]] constexpr T
convert_endian( const T value ) {
    if constexpr ( sourceEndian != targetEndian ) {
        return std::byteswap( value );
    }
    else {
        return value;
    }
}

// clang-format off
// Least Significant Bit
template <std::endian E, std::size_t N = 32> requires (N != 0) && ValidEndian<E>
constexpr inline std::size_t lsb{ (
    E == std::endian::little ? 0 : N - 1 ) };
// Most Significant Bit
template <std::endian E, std::size_t N = 32> requires (N != 0) && ValidEndian<E>
constexpr inline std::size_t msb{ (
    E == std::endian::little ? N - 1 : 0 ) };

// Consteval offset functions for compile time known offsets
template <std::endian E, std::size_t offset, std::size_t N = 32> requires (offset < N && N != 0) && ValidEndian<E>
consteval inline std::size_t lsb_offset() {
    if constexpr (E == std::endian::little) {
        return lsb<E, N> + offset;
    }
    else if constexpr (E == std::endian::big) {
        return lsb<E, N> - offset;
    }
}
template <std::endian E, std::size_t offset, std::size_t N = 32> requires (offset < N && N != 0) && ValidEndian<E>
consteval inline std::size_t msb_offset() {
    if constexpr (E == std::endian::little) {
        return msb<E, N> - offset;
    }
    else if constexpr (E == std::endian::big) {
        return msb<E, N> + offset;
    }
}

template <std::endian E, std::size_t N = 32> requires (N != 0) && ValidEndian<E>
constexpr inline std::size_t lsb_offset(const std::size_t offset) {
    if constexpr (E == std::endian::little) {
        return lsb<E, N> + offset;
    }
    else if constexpr (E == std::endian::big) {
        return lsb<E, N> - offset;
    }
}

template <std::endian E, std::size_t N = 32> requires (N != 0) && ValidEndian<E>
constexpr inline std::size_t msb_offset(const std::size_t offset) {
    if constexpr (E == std::endian::little) {
        return msb<E, N> - offset;
    }
    else if constexpr (E == std::endian::big) {
        return msb<E, N> + offset;
    }
}
// clang-format on

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 1 )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_1( const std::span<const std::byte> & data ) {
    return static_cast<T>( data[0] );
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 2 )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_2( const std::span<const std::byte> & data ) {
    if constexpr ( targetEndian == std::endian::little ) {
        return static_cast<T>( data[lsb<sourceEndian, 2>] )
               | static_cast<T>( data[msb<sourceEndian, 2>] );
    }
    else if constexpr ( targetEndian == std::endian::big ) {
        return static_cast<T>( data[msb<sourceEndian, 2>] )
               | static_cast<T>( data[lsb<sourceEndian, 2>] );
    }
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 4 )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_4( const std::span<const std::byte> & data ) {
    if constexpr ( targetEndian == std::endian::little ) {
        return static_cast<T>( data[lsb<sourceEndian, 4>] )
               | static_cast<T>( data[msb_offset<sourceEndian, 2, 4>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 1, 4>()] )
               | static_cast<T>( data[msb<sourceEndian, 4>] );
    }
    else if ( targetEndian == std::endian::big ) {
        return static_cast<T>( data[msb<sourceEndian, 4>] )
               | static_cast<T>( data[msb_offset<sourceEndian, 1, 4>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 2, 4>()] )
               | static_cast<T>( data[lsb<sourceEndian, 4>] );
    }
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 8 )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_8( const std::span<const std::byte> & data ) {
    if constexpr ( targetEndian == std::endian::little ) {
        return static_cast<T>( data[lsb<sourceEndian, 8>] )
               | static_cast<T>( data[msb_offset<sourceEndian, 6, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 5, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 4, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 3, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 2, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 1, 8>()] )
               | static_cast<T>( data[msb<sourceEndian, 8>] );
    }
    else if ( targetEndian == std::endian::big ) {
        return static_cast<T>( data[msb<sourceEndian, 8>] )
               | static_cast<T>( data[msb_offset<sourceEndian, 1, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 2, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 3, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 4, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 5, 8>()] )
               | static_cast<T>( data[msb_offset<sourceEndian, 6, 8>()] )
               | static_cast<T>( data[lsb<sourceEndian, 8>] );
    }
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T
span_to_integer( const std::span<const std::byte> & data ) {
    // Error if provided no. bytes != size of output type
    assert( sizeof( T ) == data.size() );

    // Specialisations for common integer sizes
    if constexpr ( sizeof( T ) == 1 )
        return span_to_integer_1<T, sourceEndian, targetEndian>( data );
    if constexpr ( sizeof( T ) == 2 )
        return span_to_integer_2<T, sourceEndian, targetEndian>( data );
    if constexpr ( sizeof( T ) == 4 )
        return span_to_integer_4<T, sourceEndian, targetEndian>( data );
    if constexpr ( sizeof( T ) == 8 )
        return span_to_integer<T, sourceEndian, targetEndian>( data );

    // General calculation for less common sizes
    T result{ 0 };
    for ( std::size_t i{ 0 }; i < sizeof( T ); ++i ) {
        std::size_t shift_value{ byte_bits
                                 * ( msb<targetEndian, sizeof( T )> - i ) };
        result |= static_cast<T>( data[msb<sourceEndian, sizeof( T )> - i] )
                  << shift_value;
    }

    return result;
}
