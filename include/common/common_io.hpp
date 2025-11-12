#pragma once

#include "common/common.hpp"

#include <iostream>
#include <print>
#include <sstream>
#include <string_view>

// IO concepts
template <typename T>
concept Streamable = requires( const T x ) { std::cout << x; };

template <typename T>
concept Formattable = requires( const T x ) { std::print( "{}", x ); };

template <typename T>
concept Printable = Streamable<T> && Formattable<T>;

// IO stream operator overloads
template <Streamable T, std::size_t N>
constexpr std::ostream &
operator<<( std::ostream & os, const std::array<T, N> & arr ) {
    const auto width{ os.width() };
    os.width( 2 );
    os << "{ " << std::hex;
    for ( std::size_t i{ 0 }; i < N - 1; ++i ) { os << arr[i] << ", "; }
    os << arr[N - 1] << std::dec << " }";
    os.width( width );
    return os;
}

constexpr std::ostream &
operator<<( std::ostream & out_stream, const std::byte byte ) {
    return out_stream << +std::to_integer<std::uint8_t>( byte );
}

template <IntOrEnum T, std::endian SourceEndian = std::endian::native,
          std::endian TargetEndian = SourceEndian>
    requires ValidEndian<SourceEndian>
constexpr void
print_bytes( const T value, std::ostream & out = std::cout,
             const std::string_view spacer = "'" ) {
    const auto current_flags{ out.flags() };

    out.unsetf( std::ios::showbase );
    out.setf( std::ios::hex, std::ios::basefield );

    std::string debug_msg{};
    debug_msg += "Converting: " + std::to_string( value ) + "\n";

    // 2 hex numbers / byte
    const auto bytes{ to_bytes<T, SourceEndian, TargetEndian>( value ) };
    for ( const auto [i, byte] : std::views::enumerate( bytes ) ) {
        const auto byte_num{ std::to_integer<std::uint8_t>( byte ) };

        debug_msg += "Adding: " + std::to_string( byte_num ) + "\n";
        out << byte_num;
        if ( static_cast<std::size_t>( i ) < bytes.size() - 1 ) {
            out << spacer;
        }
    }

    std::cout << debug_msg << "\n";

    out.flags( current_flags );
}
