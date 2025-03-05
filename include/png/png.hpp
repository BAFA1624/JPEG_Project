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

    // Construct PNG object from input stream
    PNG & operator<<( std::istream & input_stream );

    private:
    [[nodiscard]] std::expected<PNGChunk, png_error_t>
    parse_chunk( const std::string_view chunk_data );

    std::vector<unsigned char> png_raw;
    std::vector<PNGChunk>      png_chunks;
};

} // namespace PNG
