#pragma once

#include "png/png_types.hpp"

#include <ranges>
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
    constexpr PngChunkPayloadBase( const std::uint32_t size,
                                   const PngChunkType  chunk_type ) :
        size( size ), chunk_type( chunk_type ) {}
    constexpr virtual ~PngChunkPayloadBase() = default;
    constexpr explicit PngChunkPayloadBase(
        const PngChunkPayloadBase & other ) = default;
    constexpr PngChunkPayloadBase &
    operator=( const PngChunkPayloadBase & other ) = default;
    constexpr explicit PngChunkPayloadBase( PngChunkPayloadBase && other ) =
        default;
    constexpr PngChunkPayloadBase &
    operator=( PngChunkPayloadBase && other ) = default;

    [[nodiscard]] virtual constexpr operator bool() const noexcept {
        return is_valid( chunk_type );
    }

    [[nodiscard]] virtual constexpr bool isBaseValid() const noexcept {
        return is_valid( chunk_type );
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
        bool valid_width;
        bool valid_height;
        bool valid_bit_depth;
        bool valid_colour_type;
        bool valid_compression_method;
        bool valid_filter_method;
        bool valid_interlace_method;

        public:
        constexpr ValidChecker( const std::uint32_t     width,
                                const std::uint32_t     height,
                                const BitDepth          bit_depth,
                                const ColourType        colour_type,
                                const CompressionMethod compression_method,
                                const FilterMethod      filter_method,
                                const InterlaceMethod   interlace_method ) :
            valid_width( width > 0 ),
            valid_height( height > 0 ),
            valid_bit_depth( PNG::IHDR::is_valid( colour_type, bit_depth ) ),
            valid_colour_type( PNG::IHDR::is_valid( colour_type ) ),
            valid_compression_method(
                PNG::IHDR::is_valid( compression_method ) ),
            valid_filter_method( PNG::IHDR::is_valid( filter_method ) ),
            valid_interlace_method( PNG::IHDR::is_valid( interlace_method ) ) {}

        [[nodiscard]] constexpr bool isValid() const {
            return valid_width & valid_height && valid_bit_depth
                   && valid_colour_type && valid_compression_method
                   && valid_filter_method && valid_interlace_method;
        }
    };

    public:
    IhdrChunkPayload() = delete;
    constexpr IhdrChunkPayload(
        const std::uint32_t width, const std::uint32_t height,
        const BitDepth bit_depth, const ColourType colour_type,
        const CompressionMethod compression_method,
        const FilterMethod      filter_method,
        const InterlaceMethod   interlace_method ) noexcept :
        PngChunkPayloadBase(
            sizeof( width ) + sizeof( height ) + sizeof( bit_depth )
                + sizeof( colour_type ) + sizeof( compression_method )
                + sizeof( filter_method ) + sizeof( interlace_method ),
            PngChunkType::IHDR ),
        width( width ),
        height( height ),
        bit_depth( bit_depth ),
        colour_type( colour_type ),
        compression_method( compression_method ),
        filter_method( filter_method ),
        interlace_method( interlace_method ) {}
    explicit IhdrChunkPayload( const std::span<const std::byte> & data );

    constexpr ~IhdrChunkPayload() override = default;

    constexpr explicit IhdrChunkPayload( const IhdrChunkPayload & other ) =
        default;
    constexpr explicit IhdrChunkPayload( IhdrChunkPayload && other ) noexcept;

    constexpr IhdrChunkPayload &
    operator=( const IhdrChunkPayload & other ) = default;
    constexpr IhdrChunkPayload &
    operator=( IhdrChunkPayload && other ) noexcept;

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }

    // Overrides
    [[nodiscard]] constexpr bool isValid() const override {
        const ValidChecker valid_check( width,
                                        height,
                                        bit_depth,
                                        colour_type,
                                        compression_method,
                                        filter_method,
                                        interlace_method );
        return isBaseValid() && valid_check.isValid();
    }
    constexpr void setInvalid() noexcept override {
        setBaseInvalid();

        width = 0;
        height = 0;
        bit_depth = BitDepth{ 0 };
        colour_type = ColourType::INVALID;
        compression_method = CompressionMethod::INVALID;
        filter_method = FilterMethod::INVALID;
        interlace_method = InterlaceMethod::INVALID;
    }

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

constexpr std::vector<Palette>
bytes_to_palette( const std::span<const std::byte> & data ) {
    std::vector<Palette> result;
    result.reserve( data.size() / 3 );

    for ( const auto [i, palette_values] :
          data | std::views::chunk( sizeof( Palette ) /* = 3 */ )
              | std::views::enumerate ) {
        result[i] =
            Palette{ std::to_integer<std::uint8_t>( palette_values[0] ),
                     std::to_integer<std::uint8_t>( palette_values[1] ),
                     std::to_integer<std::uint8_t>( palette_values[2] ) };
    }

    return result;
}

class PlteChunkPayload final : protected PngChunkPayloadBase
{
    private:
    std::vector<Palette> palettes;

    protected:
    public:
    PlteChunkPayload() = delete;
    constexpr explicit PlteChunkPayload(
        const std::vector<Palette> & palettes ) :
        PngChunkPayloadBase(
            sizeof( Palette ) * static_cast<std::uint32_t>( palettes.size() ),
            PngChunkType::PLTE ),
        palettes( palettes ) {}
    constexpr explicit PlteChunkPayload(
        const std::span<const std::byte> & data ) :
        PngChunkPayloadBase( static_cast<std::uint32_t>( data.size() ),
                             PngChunkType::PLTE ),
        palettes( bytes_to_palette( data ) ) {
        if ( data.size() % 3 != 0 ) {
            setInvalid();
        }
    }
    constexpr ~PlteChunkPayload() = default;

    constexpr explicit PlteChunkPayload( const PlteChunkPayload & ) = default;
    constexpr explicit PlteChunkPayload( PlteChunkPayload && ) noexcept;

    constexpr PlteChunkPayload &
    operator=( const PlteChunkPayload & ) = default;
    constexpr PlteChunkPayload & operator=( PlteChunkPayload && ) noexcept;

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override { setBaseInvalid(); }
};

} // namespace PLTE

namespace IDAT
{

class IdatChunkPayload final : protected PngChunkPayloadBase
{
    private:
    std::vector<std::byte> m_data;

    protected:
    public:
    IdatChunkPayload() = delete;
    constexpr explicit IdatChunkPayload(
        const std::span<const std::byte> & data_span ) :
        PngChunkPayloadBase( static_cast<std::uint32_t>( data_span.size() ),
                             PngChunkType::IDAT ),
        m_data( data_span.cbegin(), data_span.cend() ) {
        assert( data_span.size() <= std::numeric_limits<std::uint32_t>::max() );
    }
    ~IdatChunkPayload() = default;
    IdatChunkPayload( const IdatChunkPayload & other ) = default;
    IdatChunkPayload & operator=( const IdatChunkPayload & other ) = default;
    IdatChunkPayload( IdatChunkPayload && other ) noexcept = default;
    IdatChunkPayload &
    operator=( IdatChunkPayload && other ) noexcept = default;

    [[nodiscard]] constexpr std::uint32_t getSize() const noexcept override {
        assert(
            m_data.size()
            == static_cast<std::uint64_t>( PngChunkPayloadBase::getSize() ) );
        return PngChunkPayloadBase::getSize();
    }

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid() && !m_data.empty() && getSize() == m_data.size();
    }
    constexpr void setInvalid() noexcept override {
        setBaseInvalid();
        m_data.clear();
    }

    [[nodiscard]] constexpr auto
    operator[]( const std::size_t i ) const noexcept {
        return m_data[i];
    }
    [[nodiscard]] constexpr auto
    operator[]( const std::size_t i, const std::size_t j,
                const bool endpoint_inclusive = false ) const noexcept {
        assert( i < j );
        return std::span{ std::next( m_data.cbegin(), i ),
                          std::next( m_data.cbegin(),
                                     j + ( endpoint_inclusive ? 1 : 0 ) ) };
    }
    [[nodiscard]] constexpr auto at( const std::size_t i ) const {
        return m_data.at( i );
    }
    [[nodiscard]] constexpr auto
    at( const std::size_t i, const std::size_t j,
        const bool endpoint_inclusive = false ) const {
        assert( i < j && i < m_data.size() && j < m_data.size() );
        return std::span{ std::next( m_data.cbegin(), i ),
                          std::next( m_data.cbegin(),
                                     j + ( endpoint_inclusive ? 1 : 0 ) ) };
    }

    [[nodiscard]] constexpr auto & data() const noexcept { return m_data; }

    // Access functions to be added?
};

} // namespace IDAT

namespace IEND
{
class IendChunkPayload final : protected PngChunkPayloadBase
{
    private:
    protected:
    public:
    constexpr IendChunkPayload() :
        PngChunkPayloadBase( 0, PngChunkType::IEND ) {}

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override { setBaseInvalid(); }
};

} // namespace IEND

// Ancillary Chunks

} // namespace PNG
