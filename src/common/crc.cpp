#include "common/crc.hpp"

#include <cstdint>

namespace CRC
{

void
CrcTable32::calculate_table() noexcept {
    value_t c;

    for ( std::uint32_t i{ 0 }; i < 256; ++i ) {
        c = value_t{ i };
        for ( std::uint32_t j{ 0 }; j < 8; ++j ) {
            if ( !( c & value_t{ 1 } ).any() ) {
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
    std::uint32_t c;
    for ( std::uint32_t i{ 0 }; i < 256; ++i ) {
        c = i;
        for ( std::uint32_t j{ 0 }; j < 8; ++j ) {
            if ( c & 1 ) {
                c = static_cast<std::uint32_t>(
                        PNG::png_polynomial<std::endian::big>().to_ulong() )
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
        return std::to_integer<std::uint32_t>( b );
    };
    constexpr auto pad_byte_swap = []( const std::byte b ) -> value_t {
        return convert_endian<std::endian::big>(
            std::to_integer<std::uint32_t>( b ) );
    };

    std::byte b{ 1 };
    b << 1;
    std::cout << "without pad_byte: "
              << std::bitset<byte_bits>{ std::to_integer<std::uint32_t>( b ) }
              << ", pad_byte: " << pad_byte( b )
              << ", pad_byte_swap: " << pad_byte_swap( b ) << std::endl;

    if ( !is_table_computed ) {
        calculate_table();
    }
    std::cout << value_t{
        convert_endian<std::endian::big, std::endian::little, std::uint32_t>(
            0xff )
    } << std::endl;

    value_t crc_result = initial_crc;
    for ( std::uint32_t i{ 0 }; i < input_bytes.size() * 8; ++i ) {
        const auto idx{ convert_endian<std::endian::big>(
            static_cast<std::uint32_t>(
                ( ( crc_result ^ pad_byte( input_bytes[i] ) )
                  & value_t{ convert_endian<std::endian::big,
                                            std::endian::little, std::uint32_t>(
                      0xff ) } )
                    .to_ulong() ) ) };
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
    return update_crc( 0xffffffffL, input_bytes ) ^ value_t { 0xffffffffL };
}

CrcTable32::value_t
CrcTable32::crc2( const std::span<const std::byte> input_bytes ) {
    return update_crc2( 0xffffffffL, input_bytes ) ^ value_t { 0xffffffffL };
}

} // namespace CRC
