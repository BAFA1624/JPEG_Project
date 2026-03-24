#include "png/png_chunk_payload_test.hpp"

#include <functional>
#include <numeric>
#include <print>
#include <string_view>

namespace PNG
{

template <PngChunkType ChunkType>
class PngChunkPayloadBaseWrapper : public PngChunkPayloadBase<ChunkType>
{
    public:
    using Base = PngChunkPayloadBase<ChunkType>;
    using Base::PngChunkPayloadBase;

    using Base::operator bool;
    using Base::getChunkType;
    using Base::isBaseValid;
    using Base::size;

    using Base::setBaseInvalid;

    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override { setBaseInvalid(); }
};

bool
test_png_chunk_payload_base() {
    constexpr auto test_valid_invalid = []() {
        return test_payload_valid<
            PngChunkPayloadBaseWrapper<PngChunkType::INVALID>>();
    };
    constexpr auto test_valid_ihdr = []() {
        return test_payload_valid<
            PngChunkPayloadBaseWrapper<PngChunkType::IHDR>>();
    };
    constexpr auto test_valid_plte = []( const std::uint32_t size ) {
        return test_payload_valid<
            PngChunkPayloadBaseWrapper<PngChunkType::PLTE>>( size );
    };
    constexpr auto test_valid_idat = []( const std::uint32_t size ) {
        return test_payload_valid<
            PngChunkPayloadBaseWrapper<PngChunkType::IDAT>>( size );
    };
    constexpr auto test_valid_iend = []() {
        return test_payload_valid<
            PngChunkPayloadBaseWrapper<PngChunkType::IEND>>();
    };

    const auto test_results = std::vector<bool>{
        // Testing correct construction w/ isValid
        TEST_INTERFACE::test_function( test_valid_invalid, false ),
        TEST_INTERFACE::test_function( test_valid_ihdr, true ),
        TEST_INTERFACE::test_function(
            test_valid_plte, true, std::uint32_t{ 0 } ),
        TEST_INTERFACE::test_function(
            test_valid_idat, true, std::uint32_t{ 0 } ),
        TEST_INTERFACE::test_function( test_valid_iend, true )
        // Testing member functions
        // isCritical/isAncillary
        // isPrivate
        // isReserved
        // isSafeToCopy
        // WIP when chunk_size_type branch works
        // getSize
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_ihdr_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<IHDR::IhdrChunkPayload>( data );
    };

    const auto test_results = std::vector<bool>{
        // Valid Chunk Tests:
        TEST_INTERFACE::test_function(
            test_valid,
            true,
            IHDR::generate_ihdr_bytes<
                std::uint32_t{ 1 },
                std::uint32_t{ 1 },
                IHDR::BitDepth{ 1 },
                IHDR::ColourType::GREYSCALE,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::ADAM_7,
                true>() ),
        TEST_INTERFACE::test_function(
            test_valid,
            true,
            IHDR::generate_ihdr_bytes<
                500,
                500,
                IHDR::BitDepth{ 16 },
                IHDR::ColourType::TRUE_COLOUR_ALPHA,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::NO_INTERLACE,
                true>() ),
        // Invalid Chunk Tests:
        // - Invalid Width
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                0,
                500,
                IHDR::BitDepth{ 16 },
                IHDR::ColourType::TRUE_COLOUR_ALPHA,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::NO_INTERLACE,
                false>() ),
        // - Invalid Height
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                500,
                0,
                IHDR::BitDepth{ 16 },
                IHDR::ColourType::TRUE_COLOUR_ALPHA,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::NO_INTERLACE,
                false>() ),
        // - Invalid BitDepth
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                500,
                500,
                IHDR::BitDepth{ 5 },
                IHDR::ColourType::TRUE_COLOUR_ALPHA,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::NO_INTERLACE,
                false>() ),
        // - Invalid ColourType 1
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                500,
                500,
                IHDR::BitDepth{ 16 },
                IHDR::ColourType::INVALID,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::NO_INTERLACE,
                false>() ),
        // - Invalid ColourType 2
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                500,
                500,
                IHDR::BitDepth{ 4 },
                IHDR::ColourType::TRUE_COLOUR,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::NO_INTERLACE,
                false>() ),
        // - Invalid CompressionMethod
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<500,
                                      500,
                                      IHDR::BitDepth{ 16 },
                                      IHDR::ColourType::GREYSCALE,
                                      IHDR::CompressionMethod::INVALID,
                                      IHDR::FilterMethod::FILTER_METHOD_0,
                                      IHDR::InterlaceMethod::NO_INTERLACE,
                                      false>() ),
        // - Invalid FilterMethod
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                500,
                500,
                IHDR::BitDepth{ 16 },
                IHDR::ColourType::GREYSCALE,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::INVALID,
                IHDR::InterlaceMethod::NO_INTERLACE,
                false>() ),
        // - Invalid InterlaceMethod
        TEST_INTERFACE::test_function(
            test_valid,
            false,
            IHDR::generate_ihdr_bytes<
                500,
                500,
                IHDR::BitDepth{ 16 },
                IHDR::ColourType::GREYSCALE,
                IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                IHDR::FilterMethod::FILTER_METHOD_0,
                IHDR::InterlaceMethod::INVALID,
                false>() )
        // Multiple Invalid Inputs:
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_plte_payload() {
    const auto test_results = std::vector<bool>{};
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_idat_payload() {
    const auto test_results = std::vector<bool>{};
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_iend_payload() {
    const auto test_results = std::vector<bool>{};
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_fail() {
    return false;
}

} // namespace PNG

int
png_chunk_payload_test( [[maybe_unused]] int     argc,
                        [[maybe_unused]] char ** argv ) {
    return TEST_INTERFACE::run_tests( "PNG Chunk Payload",
                                      PNG::test_functions );
}
