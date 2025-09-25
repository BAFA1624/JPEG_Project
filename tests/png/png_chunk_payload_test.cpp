#include "png/png_chunk_payload_test.hpp"

#include <numeric>
#include <print>

namespace PNG
{

class PngChunkPayloadBaseWrapper : public PngChunkPayloadBase
{
    public:
    using PngChunkPayloadBase::PngChunkPayloadBase;

    using PngChunkPayloadBase::operator bool;
    using PngChunkPayloadBase::getChunkType;
    using PngChunkPayloadBase::getSize;
    using PngChunkPayloadBase::isBaseValid;

    using PngChunkPayloadBase::setBaseInvalid;

    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override { setBaseInvalid(); }
};

bool
test_png_chunk_payload_base() {
    return TEST_INTERFACE::test_payload<PngChunkPayloadBaseWrapper, false>(
               std::uint32_t{ 0 }, PngChunkType::INVALID )
           && TEST_INTERFACE::test_payload<PngChunkPayloadBaseWrapper, true>(
               std::uint32_t{ 13 }, PngChunkType::IHDR )
           && TEST_INTERFACE::test_payload<PngChunkPayloadBaseWrapper, true>(
               std::uint32_t{ 0 }, PngChunkType::PLTE )
           && TEST_INTERFACE::test_payload<PngChunkPayloadBaseWrapper, true>(
               std::uint32_t{ 0 }, PngChunkType::IDAT )
           && TEST_INTERFACE::test_payload<PngChunkPayloadBaseWrapper, true>(
               std::uint32_t{ 0 }, PngChunkType::IEND );
}

bool
test_ihdr_payload() {
    std::println( "Testing IHDR Chunk Payload:" );
    constexpr auto print_result = []( const bool result, const bool expected ) {
        std::println(
            "  - Test {}{}.",
            ( result ? "Passed" : "Failed" ),
            ( result ? "" :
                       std::format( " [expected: {}]",
                                    ( expected ? "Valid" : "Invalid" ) ) ) );
    };

    bool current_result{ false };
    bool test_result{ true };

    // Valid Chunk Tests:

    current_result = TEST_INTERFACE::test_payload<IHDR::IhdrChunkPayload, true>(
        IHDR::generate_ihdr_bytes<std::uint32_t{ 1 },
                                  std::uint32_t{ 1 },
                                  IHDR::BitDepth{ 1 },
                                  IHDR::ColourType::GREYSCALE,
                                  IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                                  IHDR::FilterMethod::FILTER_METHOD_0,
                                  IHDR::InterlaceMethod::ADAM_7,
                                  true>() );
    print_result( current_result, true );
    test_result &= current_result;

    current_result = TEST_INTERFACE::test_payload<IHDR::IhdrChunkPayload, true>(
        IHDR::generate_ihdr_bytes<500,
                                  500,
                                  IHDR::BitDepth{ 16 },
                                  IHDR::ColourType::TRUE_COLOUR_ALPHA,
                                  IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                                  IHDR::FilterMethod::FILTER_METHOD_0,
                                  IHDR::InterlaceMethod::NO_INTERLACE,
                                  true>() );
    print_result( current_result, true );
    test_result &= current_result;

    // Invalid Chunk Tests:

    // Invalid Width:
    current_result = TEST_INTERFACE::test_payload<IHDR::IhdrChunkPayload,
                                                  false>(
        IHDR::generate_ihdr_bytes<0,
                                  500,
                                  IHDR::BitDepth{ 16 },
                                  IHDR::ColourType::TRUE_COLOUR_ALPHA,
                                  IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                                  IHDR::FilterMethod::FILTER_METHOD_0,
                                  IHDR::InterlaceMethod::NO_INTERLACE,
                                  false>() );
    print_result( current_result, false );
    test_result &= current_result;

    // Invalid Height:
    current_result = TEST_INTERFACE::test_payload<IHDR::IhdrChunkPayload,
                                                  false>(
        IHDR::generate_ihdr_bytes<500,
                                  0,
                                  IHDR::BitDepth{ 16 },
                                  IHDR::ColourType::TRUE_COLOUR_ALPHA,
                                  IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                                  IHDR::FilterMethod::FILTER_METHOD_0,
                                  IHDR::InterlaceMethod::NO_INTERLACE,
                                  false>() );
    print_result( current_result, false );
    test_result &= current_result;

    // Invalid BitDepth:
    current_result = TEST_INTERFACE::test_payload<IHDR::IhdrChunkPayload,
                                                  false>(
        IHDR::generate_ihdr_bytes<500,
                                  500,
                                  IHDR::BitDepth{ 4 },
                                  IHDR::ColourType::TRUE_COLOUR_ALPHA,
                                  IHDR::CompressionMethod::COMPRESSION_METHOD_0,
                                  IHDR::FilterMethod::FILTER_METHOD_0,
                                  IHDR::InterlaceMethod::NO_INTERLACE,
                                  false>() );
    print_result( current_result, false );
    test_result &= current_result;

    // Invalid ColourType:
    // Invalid CompressionMethod:
    // Invalid FilterMethod:
    // Invalid InterlaceMethod:
    // Multiple Invalid Arguments:

    return test_result;
}

bool
test_plte_payload() {
    return false;
}

bool
test_idat_payload() {
    return false;
}

bool
test_iend_payload() {
    return false;
}

} // namespace PNG

int
png_chunk_payload_test( [[maybe_unused]] int     argc,
                        [[maybe_unused]] char ** argv ) {
    return TEST_INTERFACE::run_tests( "PNG Chunk Payload",
                                      PNG::test_functions );
}
