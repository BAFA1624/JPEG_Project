#pragma once

#include "png/png_types.hpp"

#include <vector>

namespace PNG
{

// PngChunkPayloadBase: Base class to be inherited by all other Payload classes.
class PngChunkPayloadBase
{
    private:
    protected:
    std::uint32_t size;
    PngChunkType  chunk_type;

    public:
    PngChunkPayloadBase( const std::uint32_t size,
                         const PngChunkType  chunk_type ) :
        size( size ), chunk_type( chunk_type ) {}
    virtual ~PngChunkPayloadBase() = default;
    PngChunkPayloadBase( const PngChunkPayloadBase & other );

    [[nodiscard]] virtual constexpr bool isBaseValid() const noexcept {
        return chunk_type != PngChunkType::INVALID;
    }
    [[nodiscard]] virtual constexpr bool          isValid() const = 0;
    [[nodiscard]] virtual constexpr std::uint32_t getSize() const {
        return size;
    }
    [[nodiscard]] virtual constexpr PngChunkType getChunkType() const {
        return chunk_type;
    }

    constexpr void setBaseInvalid() noexcept {
        size = 0;
        chunk_type = PngChunkType::INVALID;
    }
    virtual constexpr void setInvalid() noexcept = 0;
};

// Critical Chunks:

namespace IHDR
{

class IhdrChunkPayload final : protected PngChunkPayloadBase
{
    private:
    // IHDR Chunk Members:
    std::uint32_t     width;
    std::uint32_t     height;
    BitDepth          bit_depth;
    ColourType        colour_type;
    CompressionMethod compression_method;
    FilterMethod      filter_method;
    InterlaceMethod   interlace_method;

    struct ValidChecker
    {
        private:
        bool valid_bit_depth;
        bool valid_colour_type;
        bool valid_compression_method;
        bool valid_filter_method;
        bool valid_interlace_method;

        public:
        ValidChecker( const BitDepth bit_depth, const ColourType colour_type,
                      const CompressionMethod compression_method,
                      const FilterMethod      filter_method,
                      const InterlaceMethod   interlace_method ) :
            valid_bit_depth( PNG::IHDR::is_valid( colour_type, bit_depth ) ),
            valid_colour_type( PNG::IHDR::is_valid( colour_type ) ),
            valid_compression_method(
                PNG::IHDR::is_valid( compression_method ) ),
            valid_filter_method( PNG::IHDR::is_valid( filter_method ) ),
            valid_interlace_method( PNG::IHDR::is_valid( interlace_method ) ) {
            };

        [[nodiscard]] constexpr bool isValid() const {
            return valid_bit_depth && valid_colour_type
                   && valid_compression_method && valid_filter_method
                   && valid_interlace_method;
        }
    };

    public:
    IhdrChunkPayload() = delete;
    constexpr IhdrChunkPayload(
        const std::uint32_t width, const std::uint32_t height,
        const BitDepth bit_depth, const ColourType colour_type,
        const CompressionMethod compression_method,
        const FilterMethod      filter_method,
        const InterlaceMethod   interlace_method ) noexcept;
    explicit IhdrChunkPayload(
        const std::span<const std::byte> & data ) noexcept;
    IhdrChunkPayload( const IhdrChunkPayload & other ) noexcept;
    IhdrChunkPayload & operator=( const IhdrChunkPayload & other ) noexcept;
    IhdrChunkPayload( IhdrChunkPayload && other ) noexcept;
    IhdrChunkPayload & operator=( IhdrChunkPayload && other ) noexcept;
    ~IhdrChunkPayload() override = default;

    // Overrides
    [[nodiscard]] constexpr bool isValid() const override {
        return isBaseValid() && width > 0 && height > 0;
    }
    constexpr void setInvalid() noexcept;

    // Getters
    [[nodiscard]] constexpr auto getWidth() const { return width; }
    [[nodiscard]] constexpr auto getHeight() const { return height; }
    [[nodiscard]] constexpr auto getBitDepth() const { return bit_depth; }
    [[nodiscard]] constexpr auto getColourType() const { return colour_type; }
    [[nodiscard]] constexpr auto getCompressionMethod() const {
        return compression_method;
    }
    [[nodiscard]] constexpr auto getFilterMethod() const {
        return filter_method;
    }
    [[nodiscard]] constexpr auto getInterlaceMethod() const {
        return interlace_method;
    }
};

} // namespace IHDR

namespace PLTE
{

class PlteChunkPayload final : protected PngChunkPayloadBase
{
    private:
    std::vector<Palette> palettes;

    protected:
    public:
    PlteChunkPayload() = delete;
    constexpr explicit PlteChunkPayload(
        const std::vector<Palette> & palettes );
    constexpr explicit PlteChunkPayload(
        const std::span<const std::byte> & data );
};

} // namespace PLTE

namespace IDAT
{

class IdatChunkPayload final : protected PngChunkPayloadBase
{};

} // namespace IDAT

namespace IEND
{

class IendChunkPayload final : protected PngChunkPayloadBase
{};

} // namespace IEND

// Ancillary Chunks

} // namespace PNG
