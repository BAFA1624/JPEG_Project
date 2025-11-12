#pragma once

#include <bit>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#define STRINGIFY_IMPL( x ) #x
#define STRINGIFY( x )      STRINGIFY_IMPL( x )

#define COLD [[unlikely]]

constexpr inline std::uint8_t byte_bits{ 8 };

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

template <typename T1, typename T2>
concept ComparableElements = requires( const T1 lhs, const T2 rhs ) {
    { lhs == rhs } -> std::same_as<bool>;
};

template <typename T1, typename T2, std::size_t N>
    requires ComparableElements<T1, T2>
constexpr bool
operator==( const std::array<T1, N> & lhs, const std::array<T2, N> & rhs ) {
    bool result = true;
    for ( const auto & [lhs_element, rhs_element] :
          std::views::zip( lhs, rhs ) ) {
        result &= ( lhs_element == rhs_element );
    }
    return result;
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

template <std::size_t Idx, std::integral T>
    requires( Idx < sizeof( T ) )
constexpr auto
get_byte( const T value ) {
    const auto bytes{
        std::bit_cast<const std::array<const std::byte, sizeof( T )>>( value )
    };
    return bytes[static_cast<std::size_t>( Idx )];
}

template <std::integral T>
inline auto
get_byte( const T value, const T idx ) {
    assert( idx < sizeof( T ) );
    const auto bytes{
        std::bit_cast<const std::array<const std::byte, sizeof( T )>, T>(
            value )
    };
    return bytes[static_cast<std::size_t>( idx )];
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
    const auto bytes{
        std::bit_cast<const std::array<const std::byte, sizeof( T )>>( value )
    };
    std::array<std::byte, sizeof( T )> result{};
    result[msB_offset<TargetEndian, T, 0>()] =
        bytes[msB_offset<SourceEndian, T, 0>()];
    result[msB_offset<TargetEndian, T, 1>()] =
        bytes[msB_offset<SourceEndian, T, 1>()];
    return result;
}

template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian>
    requires( sizeof( T ) == sizeof( std::uint32_t ) )
constexpr std::array<std::byte, sizeof( T )>
to_bytes_4( const T value ) {
    const auto bytes{
        std::bit_cast<const std::array<const std::byte, sizeof( T )>>( value )
    };
    std::array<std::byte, sizeof( T )> result{};
    result[msB_offset<TargetEndian, T, 0>()] =
        bytes[msB_offset<SourceEndian, T, 0>()];
    result[msB_offset<TargetEndian, T, 1>()] =
        bytes[msB_offset<SourceEndian, T, 1>()];
    result[msB_offset<TargetEndian, T, 2>()] =
        bytes[msB_offset<SourceEndian, T, 2>()];
    result[msB_offset<TargetEndian, T, 3>()] =
        bytes[msB_offset<SourceEndian, T, 3>()];
    return result;
}

template <IntOrEnum T, std::endian SourceEndian, std::endian TargetEndian>
    requires( sizeof( T ) == sizeof( std::uint64_t ) )
constexpr std::array<std::byte, sizeof( T )>
to_bytes_8( const T value ) {
    const auto bytes{
        std::bit_cast<const std::array<const std::byte, sizeof( T )>>( value )
    };
    std::array<std::byte, sizeof( T )> result{};
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 0>() )] =
        bytes[msB_offset<SourceEndian, T, 0>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 1>() )] =
        bytes[msB_offset<SourceEndian, T, 1>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 2>() )] =
        bytes[msB_offset<SourceEndian, T, 2>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 3>() )] =
        bytes[msB_offset<SourceEndian, T, 3>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 4>() )] =
        bytes[msB_offset<SourceEndian, T, 4>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 5>() )] =
        bytes[msB_offset<SourceEndian, T, 5>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 6>() )] =
        bytes[msB_offset<SourceEndian, T, 6>()];
    result[static_cast<std::size_t>( msB_offset<TargetEndian, T, 7>() )] =
        bytes[msB_offset<SourceEndian, T, 7>()];
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
        using U = integral_t<T>;

        constexpr U N{ sizeof( U ) };

        const U underlying_value{ as_integral( value ) };

        const auto bytes{
            std::bit_cast<const std::array<const std::byte, sizeof( T )>>(
                underlying_value )
        };

        std::array<std::byte, sizeof( U )> result;
        for ( U i{ 0 }; i < N; ++i ) {
            result[static_cast<std::size_t>(
                msB_offset<TargetEndian, integral_t<T>>( i ) )] =
                bytes[static_cast<std::size_t>(
                    msB_offset<SourceEndian, integral_t<T>>( i ) )];
        }

        return result;
    }
}

