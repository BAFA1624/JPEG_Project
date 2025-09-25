#pragma once

#include "png/png_chunk_payload.hpp"
#include "test_interface.hpp"

namespace TEST_INTERFACE
{
template <typename Payload, bool ExpectedResult, typename... Args>
    requires std::constructible_from<Payload, Args...>
constexpr bool
test_payload( const Args &&... args ) {
    const Payload payload{ std::forward<const Args>( args )... };
    return payload.isValid() == ExpectedResult;
}
template <typename Payload, typename... Args>
constexpr bool
test_payload( const bool expected_result, const Args &&... args ) {
    const Payload payload{ std::forward<const Args>( args )... };
    return payload.isValid() == expected_result;
}
} // namespace TEST_INTERFACE

namespace PNG
{
bool test_png_chunk_payload_base();

namespace IHDR
{

// clang-format off
/* |-----------------------------------------------------------------------------------------------------------------------|
 * | Expression | ExpectedValid | Expression && ExpectedValid | Expression || ExpectedValid | Expression || !ExpectedValid |
 * |------------|---------------|-----------------------------|-----------------------------|------------------------------|
 * |    true    |   true        |           true              |           true              |           true               |
 * |    true    |   false       |           false             |           true              |           true               |
 * |    false   |   true        |           false             |           true              |           false              |
 * |    false   |   false       |           false             |           false             |           true               |
 * |-----------------------------------------------------------------------------------------------------------------------|
 */
// clang-format on
template <bool Expression, bool ExpectedValid>
concept ValidCheck = Expression || !ExpectedValid;

template <std::uint32_t width, std::uint32_t height, BitDepth bit_depth,
          ColourType colour_type, CompressionMethod compression_method,
          FilterMethod filter_method, InterlaceMethod interlace_method,
          bool expected_valid>
    requires ValidCheck<( width > 0 ), expected_valid>
             && ValidCheck<( height > 0 ), expected_valid>
             && ValidCheck<is_valid( bit_depth ), expected_valid>
             && ValidCheck<is_valid( colour_type ), expected_valid>
             && ValidCheck<is_valid( compression_method ), expected_valid>
             && ValidCheck<is_valid( filter_method ), expected_valid>
             && ValidCheck<is_valid( interlace_method ), expected_valid>
constexpr std::vector<std::byte>
generate_ihdr_bytes() {
    constexpr auto conv_func = []<typename T>( const T value ) {
        return to_bytes<T, std::endian::native, std::endian::big>( value );
    };
    constexpr auto insert_bytes = []( std::vector<std::byte> & target,
                                      const auto &             source ) {
        target.insert( target.cend(), source.cbegin(), source.cend() );
    };

    std::vector<std::byte> bytes{};
    bytes.reserve( 13 );

    insert_bytes( bytes, conv_func( width ) );
    insert_bytes( bytes, conv_func( height ) );
    insert_bytes( bytes, conv_func( bit_depth ) );
    insert_bytes( bytes, conv_func( colour_type ) );
    insert_bytes( bytes, conv_func( compression_method ) );
    insert_bytes( bytes, conv_func( filter_method ) );
    insert_bytes( bytes, conv_func( interlace_method ) );

    return bytes;
}

} // namespace IHDR

namespace PLTE
{}

namespace IDAT
{}

namespace IEND
{}

// Critical Chunks
bool test_ihdr_payload();
bool test_plte_payload();
bool test_idat_payload();
bool test_iend_payload();

// Ancillary Chunks

const auto test_functions = std::vector{
    test_png_chunk_payload_base, test_ihdr_payload /*,
                                  test_plte_payload,
                                  test_idat_payload,
                                  test_iend_payload */
};
} // namespace PNG
