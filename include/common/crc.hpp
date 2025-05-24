#pragma once

#include "common/common.hpp"

#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <span>

namespace CRC
{

enum class crc_t { simple, table };

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
        table( std::array<value_t, 256>{} ),
        polynomial( polynomial ) {
        calculate_table();
    }

    [[nodiscard]] value_t crc( const std::span<const std::byte> input_bytes );

    private:
    void calculate_table() noexcept;
    [[nodiscard]] value_t
    update_crc( const value_t &                  initial_crc,
                const std::span<const std::byte> input_bytes ) noexcept;

    bool                     is_table_computed;
    std::array<value_t, 256> table;
    value_t                  polynomial;
};

} // namespace CRC
