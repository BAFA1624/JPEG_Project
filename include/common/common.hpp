#pragma once

#include <bit>
#include <bitset>
#include <cassert>
#include <ostream>
#include <span>

#define STRINGIFY_IMPL( x ) #x
#define STRINGIFY( x )      STRINGIFY_IMPL( x )

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

constexpr inline std::uint8_t byte_bits{ 8 };

constexpr std::ostream &
operator<<( std::ostream & out_stream, const std::byte byte ) {
    return out_stream << std::bitset<byte_bits>(
               std::to_integer<unsigned>( byte ) );
}

template <std::endian E>
concept ValidEndian = E == std::endian::little || E == std::endian::big;

template <std::endian sourceEndian,
          std::endian targetEndian = std::endian::native, std::integral T>
    requires ValidEndian<sourceEndian> && ValidEndian<targetEndian>
[[nodiscard]] constexpr T
convert_endian( const T value ) {
    if constexpr ( sourceEndian != targetEndian ) {
        return std::byteswap( value );
    }
    else {
        return value;
    }
}

// Least Significant Bit
template <typename T, std::endian E>
    requires std::is_integral_v<T> && ( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr inline T lsb{ ( E == std::endian::little ?
                              0 :
                              ( sizeof( T ) * static_cast<T>( byte_bits ) )
                                  - 1 ) };
// Most Significant Bit
template <typename T, std::endian E>
    requires std::is_integral_v<T> && ( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr inline T msb{ ( E == std::endian::little ?
                              ( sizeof( T ) * static_cast<T>( byte_bits ) )
                                  - 1 :
                              T{ 0 } ) };

// Least Significant Byte
template <typename T, std::endian E>
    requires std::is_integral_v<T> && ( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr inline T lsB{ ( E == std::endian::little ? T{ 0 } :
                                                     sizeof( T ) - 1 ) };
// Most Significant Byte
template <typename T, std::endian E>
    requires std::is_integral_v<T> && ( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr inline T msB{ ( E == std::endian::little ? sizeof( T ) - 1 :
                                                     T{ 0 } ) };

// Consteval significant bit offset functions for compile time known offsets
template <std::endian E, std::integral T, T offset>
    requires( offset < byte_bits * sizeof( T ) && sizeof( T ) != 0 )
            && ValidEndian<E>
consteval T
lsb_offset() {
    if constexpr ( E == std::endian::little ) {
        return lsb<T, E> + offset;
    }
    else {
        return lsb<T, E> - offset;
    }
}
template <std::endian E, std::integral T, T offset>
    requires( offset < byte_bits * sizeof( T ) && sizeof( T ) != 0 )
            && ValidEndian<E>
consteval T
msb_offset() {
    if constexpr ( E == std::endian::little ) {
        return msb<T, E> - offset;
    }
    else {
        return msb<T, E> + offset;
    }
}

// Consteval significant byte offset functions for compile time known offsets
template <std::endian E, std::integral T, T offset>
    requires( offset < sizeof( T ) && sizeof( T ) != 0 ) && ValidEndian<E>
consteval T
lsB_offset() {
    if constexpr ( E == std::endian::little ) {
        return offset;
    }
    else {
        return sizeof( T ) - 1 - offset;
    }
}
template <std::endian E, std::integral T, T offset>
    requires( offset < sizeof( T ) && sizeof( T ) != 0 ) && ValidEndian<E>
consteval T
msB_offset() {
    if constexpr ( E == std::endian::little ) {
        return sizeof( T ) - 1 - offset;
    }
    else {
        return offset;
    }
}

// Constexpr significant bit offset functions for runtime offsets
template <std::endian E, std::integral T>
    requires( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr T
lsb_offset( const T offset ) {
    assert( offset < byte_bits * sizeof( T ) );
    if constexpr ( E == std::endian::little ) {
        return lsb<T, E> + offset;
    }
    else if constexpr ( E == std::endian::big ) {
        return lsb<T, E> - offset;
    }
}
template <std::endian E, std::integral T>
    requires( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr T
msb_offset( const T offset ) {
    assert( offset < byte_bits * sizeof( T ) );
    if constexpr ( E == std::endian::little ) {
        return msb<T, E> - offset;
    }
    else if constexpr ( E == std::endian::big ) {
        return msb<T, E> + offset;
    }
}

// Constexpr significant byte offset functions for runtime offsets
template <std::endian E, std::integral T>
    requires( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr T
lsB_offset( const T offset ) {
    assert( offset < sizeof( T ) );
    if constexpr ( E == std::endian::little ) {
        return offset;
    }
    else {
        return static_cast<T>( sizeof( T ) - 1 - offset );
    }
}
template <std::endian E, std::integral T>
    requires( sizeof( T ) != 0 ) && ValidEndian<E>
constexpr T
msB_offset( const T offset ) {
    assert( offset < sizeof( T ) );
    if constexpr ( E == std::endian::little ) {
        return static_cast<T>( sizeof( T ) - 1 - offset );
    }
    else {
        return offset;
    }
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 1 )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_1( const std::span<const std::byte> & data ) {
    return std::bit_cast<const T>( data[0] );
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 2 )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_2( const std::span<const std::byte> & data ) {
    if constexpr ( targetEndian == std::endian::little ) {
        return static_cast<const T>( data[lsB<T, sourceEndian>] )
               | static_cast<T>(
                     data[static_cast<std::size_t>( msB<T, sourceEndian> )] )
                     << static_cast<T>( byte_bits );
    }
    else if constexpr ( targetEndian == std::endian::big ) {
        return static_cast<const T>( data[msB<T, sourceEndian>] )
               | static_cast<T>(
                     data[static_cast<std::size_t>( lsB<T, sourceEndian> )] )
                     << static_cast<T>( byte_bits );
    }
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == sizeof( std::uint32_t ) )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_4( const std::span<const std::byte> & data ) {
    if constexpr ( targetEndian == std::endian::little ) {
        return static_cast<const T>( data[lsB<T, sourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 1 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[msB<T, sourceEndian>] )
                   << static_cast<const T>( byte_bits * 3 ) );
    }
    else if constexpr ( targetEndian == std::endian::big ) {
        return static_cast<const T>( data[msB<T, sourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits * 1 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[lsB<T, sourceEndian>] )
                   << static_cast<const T>( byte_bits * 3 ) );
    }
}

template <typename T, std::endian sourceEndian,
          std::endian targetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == sizeof( std::uint64_t ) )
             && ValidEndian<sourceEndian> && ValidEndian<targetEndian>
constexpr T span_to_integer_8( const std::span<const std::byte> & data ) {
    if constexpr ( targetEndian == std::endian::little ) {
        return static_cast<const T>( data[lsB<T, sourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 6>() )] )
                   << static_cast<const T>( byte_bits ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 5>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 4>() )] )
                   << static_cast<const T>( byte_bits * 3 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 3>() )] )
                   << static_cast<const T>( byte_bits * 4 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 5 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits * 6 ) )
               | ( static_cast<const T>( data[msB<T, sourceEndian>] )
                   << static_cast<const T>( byte_bits * 7 ) );
    }
    else if constexpr ( targetEndian == std::endian::big ) {
        return static_cast<const T>( data[msB<T, sourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 3>() )] )
                   << static_cast<const T>( byte_bits * 3 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 4>() )] )
                   << static_cast<const T>( byte_bits * 4 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 5>() )] )
                   << static_cast<const T>( byte_bits * 5 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<sourceEndian, T, 6>() )] )
                   << static_cast<const T>( byte_bits * 6 ) )
               | ( static_cast<const T>(
                       data[static_cast<std::size_t>( lsB<T, sourceEndian> )] )
                   << static_cast<const T>( byte_bits * 7 ) );
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
    if constexpr ( sizeof( T ) == sizeof( std::uint8_t ) ) {
        return span_to_integer_1<T, sourceEndian, targetEndian>( data );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint16_t ) ) {
        return span_to_integer_2<T, sourceEndian, targetEndian>( data );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint32_t ) ) {
        return span_to_integer_4<T, sourceEndian, targetEndian>( data );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint64_t ) ) {
        return span_to_integer_8<T, sourceEndian, targetEndian>( data );
    }
    else {
        // General calculation for less common sizes
        T result{ 0 };
        for ( T i{ 0 }; i < sizeof( T ); ++i ) {
            const T shift_value{ static_cast<T>( byte_bits )
                                 * msB_offset<targetEndian, T>( i ) };
            result |= static_cast<const T>( data[static_cast<std::size_t>(
                          msB_offset<sourceEndian, T>( i ) )] )
                      << shift_value;
        }

        return result;
    }
}
