#include "png/png_chunk_payload_test.hpp"

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
    return !test_payload<PngChunkPayloadBaseWrapper>( std::uint32_t{ 0 },
                                                      PngChunkType::INVALID );
}

bool
test_ihdr_payload() {
    return false;
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
    return TEST_INTERFACE::run_tests( PNG::test_functions );
}
