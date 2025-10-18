#pragma once

#include <bit>
#include <bitset>
#include <cassert>
#include <ostream>
#include <span>
#include <type_traits>
#include <utility>

#define STRINGIFY_IMPL( x ) #x
#define STRINGIFY( x )      STRINGIFY_IMPL( x )

#define COLD [[unlikely]]

constexpr inline std::uint8_t byte_bits{ 8 };

constexpr std::ostream &
operator<<( std::ostream & out_stream, const std::byte byte ) {
    return out_stream << +std::to_integer<std::uint8_t>( byte );
}

template <typename T>
using decayed = std::remove_cv<std::remove_reference_t<T>>;

template <typename T>
using decayed_t = typename decayed<T>::type;

template <typename T>
concept IntOrEnum =
    std::is_integral_v<decayed_t<T>> || std::is_enum_v<decayed_t<T>>;

template <IntOrEnum T>
using integral_t =
    typename std::conditional_t<std::is_enum_v<decayed_t<T>>,
                                std::underlying_type<decayed_t<T>>,
                                decayed<T>>::type;

template <IntOrEnum T>
constexpr integral_t<T>
as_integral( const T x ) {
    if constexpr ( std::is_enum_v<decayed_t<T>> ) {
        return std::to_underlying( x );
    }
    else {
        return x;
    }
}

template <std::endian E>
concept ValidEndian = E == std::endian::little || E == std::endian::big;

template <std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native, typename T>
    requires ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
             && IntOrEnum<T>
[[nodiscard]] constexpr T
convert_endian( const T value ) {
    if constexpr ( SourceEndian != TargetEndian ) {
        return static_cast<T>( std::byteswap( as_integral( value ) ) );
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

namespace
{
template <typename T, std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 1 )
             && ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
constexpr T
span_to_integer_1( const std::span<const std::byte> & data ) {
    return std::bit_cast<const T>( data[0] );
}

template <typename T, std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == 2 )
             && ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
constexpr T
span_to_integer_2( const std::span<const std::byte> & data ) {
    if constexpr ( TargetEndian == std::endian::little ) {
        return static_cast<const T>( data[lsB<T, SourceEndian>] )
               | static_cast<T>(
                     data[static_cast<std::size_t>( msB<T, SourceEndian> )] )
                     << static_cast<T>( byte_bits );
    }
    else if constexpr ( TargetEndian == std::endian::big ) {
        return static_cast<const T>( data[msB<T, SourceEndian>] )
               | static_cast<T>(
                     data[static_cast<std::size_t>( lsB<T, SourceEndian> )] )
                     << static_cast<T>( byte_bits );
    }
}

template <typename T, std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == sizeof( std::uint32_t ) )
             && ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
constexpr T
span_to_integer_4( const std::span<const std::byte> & data ) {
    if constexpr ( TargetEndian == std::endian::little ) {
        return static_cast<const T>( data[lsB<T, SourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 1 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[msB<T, SourceEndian>] )
                   << static_cast<const T>( byte_bits * 3 ) );
    }
    else if constexpr ( TargetEndian == std::endian::big ) {
        return static_cast<const T>( data[msB<T, SourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits * 1 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[lsB<T, SourceEndian>] )
                   << static_cast<const T>( byte_bits * 3 ) );
    }
}

template <typename T, std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ( sizeof( T ) == sizeof( std::uint64_t ) )
             && ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
constexpr T
span_to_integer_8( const std::span<const std::byte> & data ) {
    if constexpr ( TargetEndian == std::endian::little ) {
        return static_cast<const T>( data[lsB<T, SourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 6>() )] )
                   << static_cast<const T>( byte_bits ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 5>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 4>() )] )
                   << static_cast<const T>( byte_bits * 3 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 3>() )] )
                   << static_cast<const T>( byte_bits * 4 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 5 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits * 6 ) )
               | ( static_cast<const T>( data[msB<T, SourceEndian>] )
                   << static_cast<const T>( byte_bits * 7 ) );
    }
    else if constexpr ( TargetEndian == std::endian::big ) {
        return static_cast<const T>( data[msB<T, SourceEndian>] )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 1>() )] )
                   << static_cast<const T>( byte_bits ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 2>() )] )
                   << static_cast<const T>( byte_bits * 2 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 3>() )] )
                   << static_cast<const T>( byte_bits * 3 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 4>() )] )
                   << static_cast<const T>( byte_bits * 4 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 5>() )] )
                   << static_cast<const T>( byte_bits * 5 ) )
               | ( static_cast<const T>( data[static_cast<std::size_t>(
                       msB_offset<SourceEndian, T, 6>() )] )
                   << static_cast<const T>( byte_bits * 6 ) )
               | ( static_cast<const T>(
                       data[static_cast<std::size_t>( lsB<T, SourceEndian> )] )
                   << static_cast<const T>( byte_bits * 7 ) );
    }
}
} // namespace

template <typename T, std::endian SourceEndian,
          std::endian TargetEndian = std::endian::native>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
             && ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
constexpr T
span_to_integer( const std::span<const std::byte> & data ) {
    // Error if provided no. bytes != size of output type
    assert( sizeof( T ) == data.size() );

    // Specialisations for common integer sizes
    if constexpr ( sizeof( T ) == sizeof( std::uint8_t ) ) {
        return span_to_integer_1<T, SourceEndian, TargetEndian>( data );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint16_t ) ) {
        return span_to_integer_2<T, SourceEndian, TargetEndian>( data );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint32_t ) ) {
        return span_to_integer_4<T, SourceEndian, TargetEndian>( data );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint64_t ) ) {
        return span_to_integer_8<T, SourceEndian, TargetEndian>( data );
    }
    else {
        // General calculation for less common sizes
        T result{ 0 };
        for ( T i{ 0 }; i < sizeof( T ); ++i ) {
            const T shift_value{ static_cast<T>( byte_bits )
                                 * msB_offset<TargetEndian, T>( i ) };
            result |= static_cast<const T>( data[static_cast<std::size_t>(
                          msB_offset<SourceEndian, T>( i ) )] )
                      << shift_value;
        }
        return result;
    }
}

template <std::size_t Idx, std::endian Target = std::endian::native,
          std::endian Source = Target, std::integral T>
    requires ValidEndian<Target> && ValidEndian<Source> && ( Idx < sizeof( T ) )
constexpr auto
get_byte( const T value ) {
    constexpr auto bytes{ std::bit_cast<std::array<std::byte, sizeof( T )>>(
        value ) };
    if constexpr ( Target == Source ) {
        return bytes[Idx];
    }
    else {
        return bytes[sizeof( T ) - 1 - Idx];
    }
}

template <std::endian Target = std::endian::native, std::endian Source = Target,
          std::integral T>
    requires ValidEndian<Target> && ValidEndian<Source>
constexpr auto
get_byte( const T value, const T idx ) {
    assert( idx < sizeof( T ) );
    constexpr auto bytes{ std::bit_cast<std::array<std::byte, sizeof( T )>>(
        value ) };
    if constexpr ( Source == Target ) {
        return bytes[idx];
    }
    else {
        return bytes[sizeof( T ) - 1 - idx];
    }
}

namespace
{
template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian>
    requires( sizeof( T ) == sizeof( std::uint8_t ) )
constexpr std::array<std::byte, sizeof( T )>
to_bytes_1( const T value ) {
    return std::array<std::byte, sizeof( T )>{ std::byte{ value } };
}

template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian>
    requires( sizeof( T ) == sizeof( std::uint16_t ) )
constexpr std::array<std::byte, sizeof( T )>
to_bytes_2( const T value ) {
    std::array<std::byte, sizeof( T )> result{};
    result[msB_offset<TargetEndian, T, 0>()] =
        get_byte<0, TargetEndian>( value );
    result[msB_offset<TargetEndian, T, 1>()] =
        get_byte<msB_offset<SourceEndian, T, 1>()>( value );
    return result;
}

template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian>
    requires( sizeof( T ) == sizeof( std::uint32_t ) )
constexpr std::array<std::byte, sizeof( T )>
to_bytes_4( const T value ) {
    std::array<std::byte, sizeof( T )> result{};
    result[msB_offset<TargetEndian, T, 0>()] =
        get_byte<msB_offset<SourceEndian, T, 0>()>( value );
    result[msB_offset<TargetEndian, T, 1>()] =
        get_byte<msB_offset<SourceEndian, T, 1>()>( value );
    result[msB_offset<TargetEndian, T, 2>()] =
        get_byte<msB_offset<SourceEndian, T, 2>()>( value );
    result[msB_offset<TargetEndian, T, 3>()] =
        get_byte<msB_offset<SourceEndian, T, 3>()>( value );
    return result;
}

template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian>
    requires( sizeof( T ) == sizeof( std::uint64_t ) )
constexpr std::array<std::byte, sizeof( T )>
to_bytes_8( const T value ) {
    std::array<std::byte, sizeof( T )> result{};
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 0>() )] =
        get_byte<msB_offset<SourceEndian, T, 0>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 1>() )] =
        get_byte<msB_offset<SourceEndian, T, 1>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 2>() )] =
        get_byte<msB_offset<SourceEndian, T, 2>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 3>() )] =
        get_byte<msB_offset<SourceEndian, T, 3>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 4>() )] =
        get_byte<msB_offset<SourceEndian, T, 4>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 5>() )] =
        get_byte<msB_offset<SourceEndian, T, 5>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 6>() )] =
        get_byte<msB_offset<SourceEndian, T, 6>()>( value );
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 7>() )] =
        get_byte<msB_offset<SourceEndian, T, 7>()>( value );
    return result;
}
} // namespace

template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian,
          bool Optimize = true>
    requires ValidEndian<SourceEndian> && ValidEndian<TargetEndian>
constexpr std::array<std::byte, sizeof( T )>
to_bytes( const T value ) {
    if constexpr ( sizeof( T ) == sizeof( std::uint8_t ) && Optimize ) {
        return to_bytes_1<integral_t<T>, SourceEndian, TargetEndian>(
            as_integral( value ) );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint16_t ) && Optimize ) {
        return to_bytes_2<integral_t<T>, SourceEndian, TargetEndian>(
            as_integral( value ) );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint32_t ) && Optimize ) {
        return to_bytes_4<integral_t<T>, SourceEndian, TargetEndian>(
            as_integral( value ) );
    }
    else if constexpr ( sizeof( T ) == sizeof( std::uint64_t ) && Optimize ) {
        return to_bytes_8<integral_t<T>, SourceEndian, TargetEndian>(
            as_integral( value ) );
    }
    else {
        std::array<std::byte, sizeof( T )> result;
        const auto underlying_value{ as_integral( value ) };

        for ( integral_t<T> i{ 0 }; i < sizeof( T ); ++i ) {
            result[static_cast<std::size_t>(
                msB_offset<TargetEndian, integral_t<T>>( i ) )] =
                get_byte( underlying_value,
                          msB_offset<SourceEndian, integral_t<T>>( i ) );
        }

        return result;
    }
}

