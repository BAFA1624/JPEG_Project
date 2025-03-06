#pragma once

#include "png_types.hpp"

#include <filesystem>
#include <string_view>
#include <vector>

namespace PNG
{

[[nodiscard]] constexpr bool
verify_png_header( const std::bitset<64> & header_bits ) noexcept;

class PNG
{
    public:
    constexpr PNG(); // Default constructor

    constexpr explicit PNG(
        const std::string_view ); // Construct from string_view
    constexpr explicit PNG( const std::filesystem::directory_entry &
                                file ); // Construct from directory entry

    // Copy constructor / assignment
    PNG( const PNG & png );
    PNG & operator=( const PNG & png );

    // Move constructor / assignment
    PNG( PNG && png );
    PNG & operator=( PNG && png );

    [[nodiscard]] static constexpr inline bool
    verify_png_header( const std::bitset<64> & header ) noexcept;

    // Construct PNG object from input stream
    [[nodiscard]] PNG & operator<<( std::istream & input_stream );

    private:
    [[nodiscard]] std::expected<PNGChunk, png_error_t>
    parse_chunk( const std::string_view chunk_data );

    [[nodiscard]] constexpr inline bool verify_header() noexcept {
        constexpr std::bitset<64> expected_header{ 0x89'504E47'0D0A'1A'0A };
        return header_bits == expected_header;
    }

    bool                      valid_png;
    std::bitset<64>           header_bits;
    std::vector<std::uint8_t> png_raw;
    std::vector<PNGChunk>     png_chunks;
};

} // namespace PNG
