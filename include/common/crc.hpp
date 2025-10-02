#pragma once

#include "common/common.hpp"

#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <span>

namespace CRC
{

constexpr inline std::size_t crc_bytes{ sizeof( std::uint32_t ) };
constexpr inline std::size_t crc_bits{ byte_bits * crc_bytes };

using crc_t = std::bitset<crc_bits>;

namespace PNG
{

constexpr std::bitset<crc_bits> png_polynomial_big_endian{
    "11101101101110001000001100100000"
};
constexpr std::bitset<crc_bits> png_polynomial_little_endian{
    convert_endian<std::endian::big, std::endian::little>(
        static_cast<std::uint32_t>( png_polynomial_big_endian.to_ulong() ) )
};

template <std::endian E = std::endian::native>
constinit auto png_polynomial = []() {
    return ( E == std::endian::little ) ? png_polynomial_little_endian :
                                          png_polynomial_big_endian;
};

} // namespace PNG

class CrcTable32
{
    public:
    CrcTable32() = delete;
    explicit CrcTable32( const crc_t & polynomial ) :
        is_table_computed( false ),
        table( std::array<crc_t, table_size>{} ),
        polynomial( polynomial ) {
        calculate_table();
    }

    [[nodiscard]] crc_t crc( const std::span<const std::byte> input_bytes );

    private:
    static constexpr std::size_t table_size{ 256 };

    void calculate_table() noexcept;
    [[nodiscard]] crc_t
    update_crc( const crc_t &                    initial_crc,
                const std::span<const std::byte> input_bytes ) noexcept;

    bool                          is_table_computed;
    std::array<crc_t, table_size> table;
    crc_t                         polynomial;
};

} // namespace CRC
