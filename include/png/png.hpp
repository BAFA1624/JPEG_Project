#pragma once

#include "common/crc.hpp"
#include "png_types.hpp"

#include <string_view>
#include <utility>
#include <vector>

namespace PNG
{

[[nodiscard]] constexpr bool
verify_png_header( const std::bitset<64> & header_bits ) noexcept {
    constexpr std::bitset<64> expected_header{ 0x89'504E47'0D0A'1A'0A };
    return header_bits == expected_header;
}

class PNG
{
    public:
    PNG() :
        valid_png( false ),
        current_error( png_error_t::NONE ),
        header_bits( 0 ),
        png_chunks( 0 ),
        crc_calculator( CRC::png_polynomial<std::endian::big>() ) {
    } // Default constructor

    explicit PNG(
        const std::string_view raw_data ); // Construct from string_view
    // constexpr explicit PNG( const std::filesystem::directory_entry &
    //                             file ); // Construct from directory entry

    // Copy constructor / assignment
    // PNG( const PNG & png );
    // PNG & operator=( const PNG & png );

    //// Move constructor / assignment
    // PNG( PNG && png );
    // PNG & operator=( PNG && png );

    [[nodiscard]] static constexpr bool
    verify_png_header( const std::bitset<64> & header ) noexcept;

    // Construct PNG object from input stream
    //[[nodiscard]] PNG & operator<<( std::istream & input_stream );

    private:
    [[nodiscard]] PNGChunk parse_chunk( const std::string_view chunk_data,
                                        std::size_t &          data_offset );

    [[nodiscard]] constexpr bool verify_header() noexcept {
        constexpr std::bitset<64> expected_header{ 0x89'504E47'0D0A'1A'0A };
        return header_bits == expected_header;
    }

    bool                  valid_png;
    png_error_t           current_error;
    std::bitset<64>       header_bits;
    std::vector<PNGChunk> png_chunks;
    CRC::CrcTable32       crc_calculator;
};

} // namespace PNG
