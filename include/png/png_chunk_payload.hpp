#pragma once

#include "png/png_types.hpp"

#include <array>

namespace PNG
{

// PngChunkPayloadBase: Base class to be inherited by all other Payload classes.
class PngChunkPayloadBase
{
    private:
    public:
    VIRTUAL_BASE_CLASS( PngChunkPayloadBase )
    [[nodiscard]] virtual constexpr std::uint32_t getSize() const = 0;
    [[nodiscard]] virtual constexpr bool          isValid() const = 0;
};

// Critical Chunks:

class IhdrChunkPayload : protected PngChunkPayloadBase
{
    private:
    constexpr static std::uint32_t m_size{ 13 };

    // IHDR Chunk Members:
    std::uint32_t     m_width;
    std::uint32_t     m_height;
    BitDepth          m_bit_depth;
    ColourType        m_colour_type;
    CompressionMethod m_compression_method;
    FilterMethod      m_filter_method;
    InterlaceMethod   m_interlace_method;

    struct ValidChecker
    {
        bool valid_bit_depth;
        bool valid_colour_type;
        bool valid_compression_method;
        bool valid_filter_method;
        bool valid_interlace_method;

        ValidChecker( const BitDepth bit_depth, const ColourType colour_type,
                      const CompressionMethod compression_method,
                      const FilterMethod      filter_method,
                      const InterlaceMethod   interlace_method ) :
            valid_colour_type( PNG::isValid( colour_type ) ),
            valid_compression_method( PNG::isValid( compression_method ) ),
            valid_filter_method( PNG::isValid( filter_method ) ),
            valid_interlace_method( PNG::isValid( interlace_method ) ) {
            if ( !valid_colour_type )
                return;

            switch ( colour_type ) {
            case ColourType::GREYSCALE: {
                valid_bit_depth = PNG::isValid( bit_depth );
            } break;
            case ColourType::INDEXED_COLOUR: {
                static constexpr std::array<BitDepth, 4> valid_bit_depths{ 1, 2,
                                                                           4,
                                                                           8 };
                valid_bit_depth =
                    PNG::isValid( bit_depth )
                    && std::ranges::contains( valid_bit_depths, bit_depth );
            } break;
            case ColourType::TRUE_COLOUR: [[fallthrough]];
            case ColourType::GREYSCALE_ALPHA: [[fallthrough]];
            case ColourType::TRUE_COLOUR_ALPHA: {
                static constexpr std::array<BitDepth, 3> valid_bit_depths{ 8,
                                                                           16 };
                valid_bit_depth =
                    PNG::isValid( bit_depth )
                    && std::ranges::contains( valid_bit_depths, bit_depth );
            } break;
            case ColourType::INVALID: [[fallthrough]];
            default: valid_bit_depth = false;
            }
        };

        [[nodiscard]] constexpr bool isValid() const {
            return valid_bit_depth && valid_colour_type
                   && valid_compression_method && valid_filter_method
                   && valid_interlace_method;
        }
    };

    public:
    IhdrChunkPayload() = delete;
    explicit IhdrChunkPayload( const std::span<const std::byte> & data );
    constexpr IhdrChunkPayload( const BitDepth          bit_depth,
                                const ColourType        colour_type,
                                const CompressionMethod compression_method,
                                const FilterMethod      filter_method,
                                const InterlaceMethod   interlace_method );
    IhdrChunkPayload( const IhdrChunkPayload & other );
    IhdrChunkPayload( IhdrChunkPayload && other );

    // Overrides
    [[nodiscard]] constexpr std::uint32_t getSize() const override {
        return m_size;
    }
    [[nodiscard]] constexpr bool isValid() const override {
        return m_width > 0 && m_height > 0;
    }

    // Getters
    [[nodiscard]] constexpr auto getWidth() const { return m_width; }
    [[nodiscard]] constexpr auto getHeight() const { return m_height; }
    [[nodiscard]] constexpr auto getBitDepth() const { return m_bit_depth; }
    [[nodiscard]] constexpr auto getColourType() const { return m_colour_type; }
    [[nodiscard]] constexpr auto getCompressionMethod() const {
        return m_compression_method;
    }
    [[nodiscard]] constexpr auto getFilterMethod() const {
        return m_filter_method;
    }
    [[nodiscard]] constexpr auto getInterlaceMethod() const {
        return m_interlace_method;
    }
};

class PlteChunkPayload : protected PngChunkPayloadBase
{};

class IdatChunkPayload : protected PngChunkPayloadBase
{};

class IendChunkPayload : protected PngChunkPayloadBase
{};

// Ancillary Chunks

} // namespace PNG
