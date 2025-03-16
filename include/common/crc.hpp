#pragma once

#include <bit>
#include <bitset>
#include <cstdint>
namespace PNG
{
namespace CRC
{
constinit inline std::bitset<32> png_polynomial{
    "00000100110000010001110110110111"
};
}
} // namespace PNG

namespace CRC
{
enum class crc_type_t { naive };

template <std::uint64_t N, std::uint64_t M, std::endian E = std::endian::native>
    requires( N <= M )
[[nodiscard]] constexpr std::bitset<N>
naive_endian( const std::uint8_t * input_bits, const std::uint64_t n_input_bits,
              const std::bitset<M> polynomial ) {
    std::bitset<N> remainder{ 0 };
    if constexpr ( E == std::endian::little ) {
        for ( std::uint64_t i{ 0 }; i < n_input_bits; ++i ) {}
    }
    else if constexpr ( E == std::endian::big ) {}
    return remainder;
}
} // namespace CRC
