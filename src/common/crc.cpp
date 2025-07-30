#include "common/crc.hpp"

#include <cstdint>
#include <ranges>

namespace CRC
{

void
CrcTable32::calculate_table() noexcept {
    for ( std::uint32_t i{ 0 }; i < 256; ++i ) {
        auto c = value_t{ i };
        for ( std::uint32_t j{ 0 }; j < 8; ++j ) {
            c = ( c >> 1 )
                ^ ( ( c & value_t{ 1 } )
                            .test( lsb<std::uint32_t, std::endian::native> ) ?
                        polynomial :
                        0 );
        }

        table[i] = c;
    }

    is_table_computed = true;
}

CrcTable32::value_t
CrcTable32::update_crc(
    const value_t &                  initial_crc,
    const std::span<const std::byte> input_bytes ) noexcept {
    constexpr auto pad_byte = []( const std::byte b ) -> value_t {
        return std::to_integer<std::uint32_t>( b );
    };

    if ( !is_table_computed ) {
        calculate_table();
    }

    auto crc = initial_crc;
    for ( const auto & [i, byte] : input_bytes | std::views::enumerate ) {
        const auto idx{
            ( ( crc ^ pad_byte( byte ) ) & value_t{ 0xFF } ).to_ulong()
        };
        crc = ( crc >> 8 ) ^ table[idx];
    }

    return crc;
}

CrcTable32::value_t
CrcTable32::crc( const std::span<const std::byte> input_bytes ) {
    return update_crc( 0xffffffffL, input_bytes ) ^ value_t { 0xffffffffL };
}

} // namespace CRC
