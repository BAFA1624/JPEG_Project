#pragma once

#include "common/common.hpp"

#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <span>

namespace CRC
{

constexpr inline std::size_t crc_bits{ 32 };

namespace PNG
{


constexpr inline std::bitset<crc_bits> png_polynomial_big_endian{
    "11101101101110001000001100100000"
};
constexpr inline std::bitset<crc_bits> png_polynomial_little_endian{
    convert_endian<std::endian::big, std::endian::little>(
        static_cast<std::uint32_t>( png_polynomial_big_endian.to_ulong() ) )
};
template <std::endian E = std::endian::native>
constinit inline auto png_polynomial = []() {
    return ( E == std::endian::little ) ? png_polynomial_little_endian :
                                          png_polynomial_big_endian;
};
} // namespace PNG

class CrcTable32
{
    public:
    using value_t = std::bitset<crc_bits>;

    CrcTable32() = delete;
    explicit CrcTable32( const value_t & polynomial ) :
        is_table_computed( false ),
        is_table2_computed( false ),
        table( std::array<value_t, 256>{} ),
        table2( std::array<std::uint64_t, 256>{} ),
        polynomial( polynomial ) {
        calculate_table();
        calculate_table2();

        // std::cout << "Table 1:\n";
        // print_table( table );
        // std::cout << "Table 2:\n";
        // print_table2( table2 );
    }

    [[nodiscard]] value_t crc( const std::span<const std::byte> input_bytes );
    [[nodiscard]] value_t crc2( const std::span<const std::byte> input_bytes );

    constexpr inline void
    print_table( const std::array<value_t, 256> & table ) const noexcept {
        for ( std::size_t i{ 0 }; i < 16; ++i ) {
            for ( std::size_t j{ 0 }; j < 16; ++j ) {
                std::cout << table[16 * i + j].to_ulong() << " ";
            }
            std::cout << "\n";
        }
    }
    constexpr inline void print_table2(
        const std::array<std::uint64_t, 256> & table ) const noexcept {
        for ( std::size_t i{ 0 }; i < 16; ++i ) {
            for ( std::size_t j{ 0 }; j < 16; ++j ) {
                std::cout << table[16 * i + j] << " ";
            }
            std::cout << "\n";
        }
    }

    private:
    void    calculate_table() noexcept;
    void    calculate_table2() noexcept;
    value_t update_crc( const value_t &                  initial_crc,
                        const std::span<const std::byte> input_bytes ) noexcept;
    value_t
    update_crc2( const value_t &                  initial_crc,
                 const std::span<const std::byte> input_bytes ) noexcept;

    bool                           is_table_computed;
    bool                           is_table2_computed;
    std::array<value_t, 256>       table;
    std::array<std::uint64_t, 256> table2;
    value_t                        polynomial;
};
} // namespace CRC
