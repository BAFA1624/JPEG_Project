#include "common/crc.hpp"

#include "common/common.hpp"

#include <cstdint>

namespace CRC
{

void
CrcTable32::calculate_table() noexcept {
    value_t c;

    for ( std::uint32_t i{ 0 }; i < 256; ++i ) {
        c = value_t( byteswap( i ) );
        for ( std::uint32_t j{ 0 }; j < 8; ++j ) {
            if ( !( c & value_t{}.set() ).any() ) {
                c = polynomial ^ ( c >> 1 );
            }
            else {
                c >>= 1;
            }
        }

        table[i] = c;
    }

    is_table_computed = true;
}

void
CrcTable32::calculate_table2() noexcept {
    std::uint64_t c;
    for ( std::uint64_t i{ 0 }; i < 256; ++i ) {
        c = i;
        for ( std::uint64_t j{ 0 }; j < 8; ++j ) {
            if ( c & 1 ) {
                c = png_polynomial<std::endian::little>().to_ulong()
                    ^ ( c >> 1 );
            }
            else {
                c = c >> 1;
            }
        }
        table2[i] = c;
    }
    is_table2_computed = true;
}

CrcTable32::value_t
CrcTable32::update_crc(
    const value_t &                  initial_crc,
    const std::span<const std::byte> input_bytes ) noexcept {
    constexpr auto pad_byte = []( const std::byte b ) -> value_t {
        return byteswap( std::to_integer<std::uint64_t>( b ) );
    };

    if ( !is_table_computed ) {
        calculate_table();
    }

    value_t crc_result = initial_crc;
    for ( std::uint64_t i{ 0 }; i < input_bytes.size() * 8; ++i ) {
        const auto idx{ ( ( crc_result ^ pad_byte( input_bytes[i] ) )
                          & value_t{ 0xff } )
                            .to_ulong() };
        // std::cout << idx << std::endl;
        crc_result = table[idx];
    }

    return crc_result;
}

CrcTable32::value_t
CrcTable32::update_crc2(
    const value_t &                  initial_crc,
    const std::span<const std::byte> input_bytes ) noexcept {
    unsigned long c;
    if ( !is_table2_computed ) {
        calculate_table2();
    }
    for ( int n{ 0 }; n < 256; ++n ) {
        c = table2[( c ^ std::to_integer<unsigned long>( input_bytes[n] ) )
                   & 0xffL]
            ^ ( c >> 8 );
    }
    return c;
}

CrcTable32::value_t
CrcTable32::crc( const std::span<const std::byte> input_bytes ) {
    return update_crc( 0xffffffffL, input_bytes );
}

CrcTable32::value_t
CrcTable32::crc2( const std::span<const std::byte> input_bytes ) {
    return update_crc2( 0xffffffffL, input_bytes );
}

} // namespace CRC
