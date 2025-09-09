#include "png/png_types_test.hpp"

#include "common/test_interfaces.hpp"

namespace PNG
{

constexpr bool
test_png_types() {
    constexpr auto png_chunk_type_test_set =
        std::array{ std::tuple{ PngChunkType::INVALID, false },
                    std::tuple{ PngChunkType::IHDR, true },
                    std::tuple{ PngChunkType::PLTE, true },
                    std::tuple{ PngChunkType::IDAT, true },
                    std::tuple{ PngChunkType::IEND, true },
                    std::tuple{ PngChunkType::bKGD, true },
                    std::tuple{ PngChunkType::cHRM, true },
                    std::tuple{ PngChunkType::dSIG, true },
                    std::tuple{ PngChunkType::eXIF, true },
                    std::tuple{ PngChunkType::gAMA, true },
                    std::tuple{ PngChunkType::hIST, true },
                    std::tuple{ PngChunkType::iCCP, true },
                    std::tuple{ PngChunkType::iTXt, true },
                    std::tuple{ PngChunkType::pHYs, true },
                    std::tuple{ PngChunkType::sBIT, true },
                    std::tuple{ PngChunkType::sPLT, true },
                    std::tuple{ PngChunkType::sRGB, true },
                    std::tuple{ PngChunkType::sTER, true },
                    std::tuple{ PngChunkType::tEXt, true },
                    std::tuple{ PngChunkType::tIME, true },
                    std::tuple{ PngChunkType::tRNS, true },
                    std::tuple{ PngChunkType::zTXt, true },
                    std::tuple{ PngChunkType{ 0 }, false },
                    std::tuple{ PngChunkType{ 121241125 }, false } };

    constexpr auto png_pixel_format_test_set =
        std::array{ std::tuple{ PngPixelFormat::grayscale, true },
                    std::tuple{ PngPixelFormat::truecolor, true },
                    std::tuple{ PngPixelFormat::indexed, true },
                    std::tuple{ PngPixelFormat::alpha_grayscale, true },
                    std::tuple{ PngPixelFormat::alpha_truecolor, true },
                    std::tuple{ PngPixelFormat{ 1 }, false },
                    std::tuple{ PngPixelFormat{ 5 }, false },
                    std::tuple{ PngPixelFormat{ 7 }, false } };

    return validate_type<PngChunkType>(
               png_chunk_type_test_set,
               []( const PngChunkType x ) { return is_valid( x ); } )
           && validate_type<PngPixelFormat>(
               png_pixel_format_test_set,
               []( const PngPixelFormat x ) { return is_valid( x ); } );
}

constexpr bool
test_ihdr_types() {
    // constexpr auto bit_depth_test_set = std::array{};

    constexpr auto colour_type_test_set =
        std::array{ std::tuple{ IHDR::ColourType::GREYSCALE, true },
                    std::tuple{ IHDR::ColourType::TRUE_COLOUR, true },
                    std::tuple{ IHDR::ColourType::INDEXED_COLOUR, true },
                    std::tuple{ IHDR::ColourType::GREYSCALE_ALPHA, true },
                    std::tuple{ IHDR::ColourType::TRUE_COLOUR_ALPHA, true },
                    std::tuple{ IHDR::ColourType::INVALID, false },
                    std::tuple{ IHDR::ColourType{ 1 }, false },
                    std::tuple{ IHDR::ColourType{ 5 }, false },
                    std::tuple{ IHDR::ColourType{ 8 }, false } };

    return true;
}

constexpr bool
test_plte_types() {
    return true;
}

} // namespace PNG

int
png_types_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t test_passes{ 0 };

    for ( const auto & func : PNG::test_functions ) {
        test_passes += ( func() ? 1 : 0 );
    }

    return static_cast<int>( PNG::test_functions.size() - test_passes );
}
