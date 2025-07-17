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
    \ class() = default;            \
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
template <std::endian baseEndianness, std::size_t N = 32> requires (N != 0)
constexpr inline std::size_t lsb{ (
    baseEndianness == std::endian::little ? 0 : N - 1 ) };
// Most Significant Bit
template <std::endian baseEndianness, std::size_t N = 32> requires (N != 0)
constexpr inline std::size_t msb{ (
    baseEndianness == std::endian::little ? N - 1 : 0 ) };

template <std::endian baseEndianness, std::size_t offset, std::size_t N = 32> requires (offset < N && N != 0)
consteval inline std::size_t lsb_offset() {
    if constexpr (baseEndianness == std::endian::little)
        return lsb<baseEndianness, N> + offset;
    else if constexpr (baseEndianness == std::endian::big)
        return lsb<baseEndianness, N> - offset;
    else
        static_assert(false, "Mixed endian formats are unsupported.");
}
template <std::endian baseEndianness, std::size_t offset, std::size_t N = 32> requires (offset < N && N != 0)
consteval inline std::size_t msb_offset() {
    if constexpr (baseEndianness == std::endian::little)
        return msb<baseEndianness, N> - offset;
    else if constexpr (baseEndianness == std::endian::big)
        return msb<baseEndianness, N> + offset;
    else
        static_assert(false, "Mixed endian formats are unsupported.");

}
// clang-format on

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 1 )
constexpr T span_to_integer_1( const std::span<std::byte> & data ) {
    return static_cast<T>( data[0] );
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 2 )
constexpr T span_to_integer_2( const std::span<std::byte> & data ) {
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
constexpr T span_to_integer_4( const std::span<std::byte> & data ) {
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
constexpr T span_to_integer_8( const std::span<std::byte> & data ) {
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
constexpr T
span_to_integer( const std::span<std::byte> & data ) {
    static_assert( sizeof( T ) == data.size() );

    // Error if mixed endian detected
    if ( ( targetEndian != std::endian::little
           && targetEndian != std::endian::big )
         || ( sourceEndian != std::endian::little
              && sourceEndian != std::endian::big ) ) {
        static_assert( false );
    }

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
