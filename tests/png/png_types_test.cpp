#include "png/png_types_test.hpp"

#include "common/test_interfaces.hpp"

namespace PNG
{

bool
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
               []( const PngChunkType input ) { return is_valid( input ); } )
           && validate_type<PngPixelFormat>(
               png_pixel_format_test_set,
               []( const PngPixelFormat input ) { return is_valid( input ); } );
}

bool
test_ihdr_types() {
    constexpr auto bit_depth_test_set =
        std::array{ std::tuple{ IHDR::BitDepth{ 1 }, true },
                    std::tuple{ IHDR::BitDepth{ 2 }, true },
                    std::tuple{ IHDR::BitDepth{ 4 }, true },
                    std::tuple{ IHDR::BitDepth{ 8 }, true },
                    std::tuple{ IHDR::BitDepth{ 16 }, true } };

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


    constexpr auto f = []( const IHDR::ColourType colour_type,
                           const IHDR::BitDepth bit_depth, const bool result )
        -> std::tuple<IHDR::ColourType, IHDR::BitDepth, bool> {
        return std::make_tuple( colour_type, bit_depth, result );
    };
    constexpr auto colour_type_bit_depth_test_set = std::array{
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 1 }, true ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 2 }, true ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 4 }, true ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 8 }, true ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 16 }, true ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 0 }, false ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 5 }, false ),
        f( IHDR::ColourType::GREYSCALE, IHDR::BitDepth{ 17 }, false ),
        f( IHDR::ColourType::TRUE_COLOUR, IHDR::BitDepth{ 8 }, true ),
        f( IHDR::ColourType::TRUE_COLOUR, IHDR::BitDepth{ 16 }, true ),
        f( IHDR::ColourType::TRUE_COLOUR, IHDR::BitDepth{ 0 }, false ),
        f( IHDR::ColourType::TRUE_COLOUR, IHDR::BitDepth{ 5 }, false ),
        f( IHDR::ColourType::TRUE_COLOUR, IHDR::BitDepth{ 17 }, false ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 1 }, true ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 2 }, true ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 4 }, true ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 8 }, true ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 0 }, false ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 5 }, false ),
        f( IHDR::ColourType::INDEXED_COLOUR, IHDR::BitDepth{ 17 }, false ),
        f( IHDR::ColourType::GREYSCALE_ALPHA, IHDR::BitDepth{ 8 }, true ),
        f( IHDR::ColourType::GREYSCALE_ALPHA, IHDR::BitDepth{ 16 }, true ),
        f( IHDR::ColourType::GREYSCALE_ALPHA, IHDR::BitDepth{ 0 }, false ),
        f( IHDR::ColourType::GREYSCALE_ALPHA, IHDR::BitDepth{ 5 }, false ),
        f( IHDR::ColourType::GREYSCALE_ALPHA, IHDR::BitDepth{ 17 }, false ),
        f( IHDR::ColourType::TRUE_COLOUR_ALPHA, IHDR::BitDepth{ 8 }, true ),
        f( IHDR::ColourType::TRUE_COLOUR_ALPHA, IHDR::BitDepth{ 16 }, true ),
        f( IHDR::ColourType::TRUE_COLOUR_ALPHA, IHDR::BitDepth{ 0 }, false ),
        f( IHDR::ColourType::TRUE_COLOUR_ALPHA, IHDR::BitDepth{ 5 }, false ),
        f( IHDR::ColourType::TRUE_COLOUR_ALPHA, IHDR::BitDepth{ 17 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 1 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 2 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 4 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 8 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 16 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 0 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 5 }, false ),
        f( IHDR::ColourType::INVALID, IHDR::BitDepth{ 17 }, false )
    };

    constexpr auto compression_method_test_set =
        std::array{ std::tuple{ IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                                true },
                    std::tuple{ IHDR::CompressionMethod::INVALID, false },
                    std::tuple{ IHDR::CompressionMethod{ 2 }, false } };

    constexpr auto filter_method_test_set =
        std::array{ std::tuple{ IHDR::FilterMethod::FILTER_METHOD_0, true },
                    std::tuple{ IHDR::FilterMethod::INVALID, false },
                    std::tuple{ IHDR::FilterMethod{ 2 }, false } };

    constexpr auto interlace_method_test_set =
        std::array{ std::tuple{ IHDR::InterlaceMethod::NO_INTERLACE, true },
                    std::tuple{ IHDR::InterlaceMethod::ADAM_7, true },
                    std::tuple{ IHDR::InterlaceMethod::INVALID, false },
                    std::tuple{ IHDR::InterlaceMethod{ 3 }, false } };

    return validate_type<IHDR::BitDepth>( bit_depth_test_set,
                                          []( const IHDR::BitDepth input ) {
                                              return IHDR::is_valid( input );
                                          } )
           && validate_type<IHDR::ColourType>(
               colour_type_test_set,
               []( const IHDR::ColourType input ) {
                   return IHDR::is_valid( input );
               } )
           && validate_type<IHDR::ColourType, IHDR::BitDepth>(
               colour_type_bit_depth_test_set,
               []( const IHDR::ColourType input1,
                   const IHDR::BitDepth   input2 ) {
                   return IHDR::is_valid( input1, input2 );
               } )
           && validate_type<IHDR::CompressionMethod>(
               compression_method_test_set,
               []( const IHDR::CompressionMethod input ) {
                   return IHDR::is_valid( input );
               } )
           && validate_type<IHDR::FilterMethod>(
               filter_method_test_set,
               []( const IHDR::FilterMethod input ) {
                   return IHDR::is_valid( input );
               } )
           && validate_type<IHDR::InterlaceMethod>(
               interlace_method_test_set,
               []( const IHDR::InterlaceMethod input ) {
                   return IHDR::is_valid( input );
               } );
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
