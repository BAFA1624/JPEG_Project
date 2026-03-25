#pragma once

#include "png/png_types.hpp"

#include <ranges>
#include <vector>

namespace PNG
{

/* TODO: Adapt for ChunkSizeType
 *  -
 */

// PngChunkPayloadBase: Base class to be inherited by all other Payload classes.
template <PngChunkType ChunkType>
class PngChunkPayloadBase
{
    private:
    PngChunkSize<ChunkType> m_size_type;

    // PngChunkTypes are always 4 bytes.
    // The property bits are bit 5 in each byte.
    template <std::size_t Byte>
        requires( Byte == 0 || Byte == 1 || Byte == 2 || Byte == 3 )
    [[nodiscard]] constexpr bool test_property_bit() const noexcept {
        return std::bitset<sizeof( PngChunkType ) * byte_bits>{ as_integral(
            ChunkType ) }[byte_bits * Byte + 5];
    }

    protected:
    public:
    constexpr PngChunkPayloadBase()
        requires( PngChunkSize<ChunkType>::sizeType()
                  == ChunkSizeType::Constant )
        : m_size_type( PngChunkSize<ChunkType>() ) {}
    constexpr PngChunkPayloadBase( const std::uint32_t size )
        requires( PngChunkSize<ChunkType>::sizeType()
                  == ChunkSizeType::Variable )
        : m_size_type( PngChunkSize<ChunkType>( size ) ) {}
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
        return is_valid( ChunkType ) && m_size_type.isValid();
    }

    [[nodiscard]] virtual constexpr bool isBaseValid() const noexcept {
        return is_valid( ChunkType ) && m_size_type.isValid();
    }

    [[nodiscard]] constexpr auto isCritical() const noexcept {
        return !test_property_bit<0>();
    }
    [[nodiscard]] constexpr auto isAncillary() const noexcept {
        return test_property_bit<0>();
    }

    [[nodiscard]] constexpr auto isPrivate() const noexcept {
        return test_property_bit<1>();
    }

    // If a chunk is not reserved:
    //   -> Chunk defined for more recent PNG specification
    //      than this implementation uses.
    //   -> Chunk defined for an unofficial PNG specification.
    [[nodiscard]] constexpr auto isReserved() const noexcept {
        return test_property_bit<2>();
    }

    [[nodiscard]] constexpr auto isSafeToCopy() const noexcept {
        return test_property_bit<3>();
    }

    [[nodiscard]] virtual constexpr bool          isValid() const = 0;
    [[nodiscard]] virtual constexpr std::uint32_t size() const {
        return m_size_type.size();
    }
    [[nodiscard]] virtual constexpr PngChunkType getChunkType() const {
        return ChunkType;
    }

    constexpr void setBaseInvalid() noexcept { m_size_type.setInvalid(); }
    virtual constexpr void setInvalid() noexcept = 0;
};

// Critical Chunks:

namespace IHDR
{

class IhdrChunkPayload final : protected PngChunkPayloadBase<PngChunkType::IHDR>
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
    using PngChunkPayloadBase<PngChunkType::IHDR>::getChunkType;
    using PngChunkPayloadBase<PngChunkType::IHDR>::size;

    IhdrChunkPayload() = delete;
    constexpr IhdrChunkPayload(
        const std::uint32_t width, const std::uint32_t height,
        const BitDepth bit_depth, const ColourType colour_type,
        const CompressionMethod compression_method,
        const FilterMethod      filter_method,
        const InterlaceMethod   interlace_method ) noexcept :
        PngChunkPayloadBase(),
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
    constexpr explicit IhdrChunkPayload( IhdrChunkPayload && other ) noexcept :
        PngChunkPayloadBase(),
        width( other.width ),
        height( other.height ),
        bit_depth( other.bit_depth ),
        colour_type( other.colour_type ),
        compression_method( other.compression_method ),
        filter_method( other.filter_method ),
        interlace_method( other.interlace_method ) {
        other.setInvalid();
    }

    constexpr IhdrChunkPayload &
    operator=( const IhdrChunkPayload & other ) = default;
    constexpr IhdrChunkPayload & operator=( IhdrChunkPayload && other ) noexcept {
        if ( this != &other ) {
            PngChunkPayloadBase::operator=( std::move( other ) );

            width = other.width;
            height = other.height;
            bit_depth = other.bit_depth;
            colour_type = other.colour_type;
            compression_method = other.compression_method;
            filter_method = other.filter_method;
            interlace_method = other.interlace_method;

            other.setInvalid();
        }

        return *this;
    }

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

std::vector<Palette>
bytes_to_palette( const std::span<const std::byte> & data );

class PlteChunkPayload final : protected PngChunkPayloadBase<PngChunkType::PLTE>
{
    private:
    std::vector<colour_t> r_channel;
    std::vector<colour_t> g_channel;
    std::vector<colour_t> b_channel;

    protected:
    public:
    using PngChunkPayloadBase<PngChunkType::PLTE>::getChunkType;
    using PngChunkPayloadBase<PngChunkType::PLTE>::size;

    PlteChunkPayload() = delete;
    explicit PlteChunkPayload(
        const std::vector<Palette> & palettes );
    explicit PlteChunkPayload(
        const std::span<const std::byte> & data );
    constexpr ~PlteChunkPayload() = default;

    constexpr explicit PlteChunkPayload( const PlteChunkPayload & ) = default;
    explicit PlteChunkPayload( PlteChunkPayload && ) noexcept;

    constexpr PlteChunkPayload &
    operator=( const PlteChunkPayload & ) = default;
    PlteChunkPayload & operator=( PlteChunkPayload && ) noexcept;

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override {
        setBaseInvalid();
        r_channel.clear();
        g_channel.clear();
        b_channel.clear();
    }

    constexpr auto operator[]( const std::size_t idx ) const noexcept {
        return Palette{ .red = r_channel[idx],
                        .green = g_channel[idx],
                        .blue = b_channel[idx] };
    }

    [[nodiscard]] constexpr const auto & rChannel() const noexcept {
        return r_channel;
    }
    [[nodiscard]] constexpr const auto & gChannel() const noexcept {
        return g_channel;
    }
    [[nodiscard]] constexpr const auto & bChannel() const noexcept {
        return b_channel;
    }

    std::vector<Palette> getPalettes() const noexcept;
};

} // namespace PLTE

namespace IDAT
{

class IdatChunkPayload final : protected PngChunkPayloadBase<PngChunkType::IDAT>
{
    private:
    std::vector<std::byte> m_data;

    protected:
    public:
    using PngChunkPayloadBase<PngChunkType::IDAT>::getChunkType;

    IdatChunkPayload() = delete;
    constexpr explicit IdatChunkPayload(
        const std::span<const std::byte> & data_span ) :
        PngChunkPayloadBase( static_cast<std::uint32_t>( data_span.size() ) ),
        m_data( data_span.cbegin(), data_span.cend() ) {
        assert( data_span.size() <= std::numeric_limits<std::uint32_t>::max() );
    }
    ~IdatChunkPayload() = default;
    IdatChunkPayload( const IdatChunkPayload & other ) = default;
    IdatChunkPayload & operator=( const IdatChunkPayload & other ) = default;
    IdatChunkPayload( IdatChunkPayload && other ) noexcept = default;
    IdatChunkPayload &
    operator=( IdatChunkPayload && other ) noexcept = default;

    [[nodiscard]] constexpr std::uint32_t size() const noexcept override {
        assert( m_data.size()
                == static_cast<std::uint64_t>( PngChunkPayloadBase::size() ) );
        return PngChunkPayloadBase::size();
    }

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid() && !m_data.empty() && size() == m_data.size();
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
class IendChunkPayload final : protected PngChunkPayloadBase<PngChunkType::IEND>
{
    private:
    protected:
    public:
    using PngChunkPayloadBase<PngChunkType::IEND>::getChunkType;
    using PngChunkPayloadBase<PngChunkType::IEND>::size;

    constexpr IendChunkPayload() : PngChunkPayloadBase() {}
    constexpr explicit IendChunkPayload( const std::span<const std::byte> & data )
        : PngChunkPayloadBase() {
        if ( !data.empty() ) {
            setInvalid();
        }
    }

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override { setBaseInvalid(); }
};

} // namespace IEND

namespace cHRM
{

class ChrmChunkPayload final : protected PngChunkPayloadBase<PngChunkType::cHRM>
{
    private:
    std::uint32_t m_white_point_x;
    std::uint32_t m_white_point_y;
    std::uint32_t m_red_x;
    std::uint32_t m_red_y;
    std::uint32_t m_green_x;
    std::uint32_t m_green_y;
    std::uint32_t m_blue_x;
    std::uint32_t m_blue_y;

    public:
    using PngChunkPayloadBase<PngChunkType::cHRM>::getChunkType;
    using PngChunkPayloadBase<PngChunkType::cHRM>::size;

    ChrmChunkPayload() = delete;
    constexpr ChrmChunkPayload( const std::uint32_t white_point_x,
                                const std::uint32_t white_point_y,
                                const std::uint32_t red_x,
                                const std::uint32_t red_y,
                                const std::uint32_t green_x,
                                const std::uint32_t green_y,
                                const std::uint32_t blue_x,
                                const std::uint32_t blue_y ) noexcept :
        PngChunkPayloadBase(),
        m_white_point_x( white_point_x ),
        m_white_point_y( white_point_y ),
        m_red_x( red_x ),
        m_red_y( red_y ),
        m_green_x( green_x ),
        m_green_y( green_y ),
        m_blue_x( blue_x ),
        m_blue_y( blue_y ) {}
    explicit ChrmChunkPayload( const std::span<const std::byte> & data );

    constexpr ~ChrmChunkPayload() override = default;
    constexpr explicit ChrmChunkPayload( const ChrmChunkPayload & other ) =
        default;
    explicit ChrmChunkPayload( ChrmChunkPayload && other ) noexcept;

    constexpr ChrmChunkPayload &
    operator=( const ChrmChunkPayload & other ) = default;
    ChrmChunkPayload & operator=( ChrmChunkPayload && other ) noexcept;

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid();
    }
    constexpr void setInvalid() noexcept override {
        setBaseInvalid();
        m_white_point_x = 0;
        m_white_point_y = 0;
        m_red_x = 0;
        m_red_y = 0;
        m_green_x = 0;
        m_green_y = 0;
        m_blue_x = 0;
        m_blue_y = 0;
    }

    [[nodiscard]] constexpr auto whitePointX() const noexcept {
        return m_white_point_x;
    }
    [[nodiscard]] constexpr auto whitePointY() const noexcept {
        return m_white_point_y;
    }
    [[nodiscard]] constexpr auto redX() const noexcept { return m_red_x; }
    [[nodiscard]] constexpr auto redY() const noexcept { return m_red_y; }
    [[nodiscard]] constexpr auto greenX() const noexcept { return m_green_x; }
    [[nodiscard]] constexpr auto greenY() const noexcept { return m_green_y; }
    [[nodiscard]] constexpr auto blueX() const noexcept { return m_blue_x; }
    [[nodiscard]] constexpr auto blueY() const noexcept { return m_blue_y; }
};

} // namespace cHRM

namespace gAMA
{

class GamaChunkPayload final : protected PngChunkPayloadBase<PngChunkType::gAMA>
{
    private:
    std::uint32_t m_image_gamma;

    public:
    using PngChunkPayloadBase<PngChunkType::gAMA>::getChunkType;
    using PngChunkPayloadBase<PngChunkType::gAMA>::size;

    GamaChunkPayload() = delete;
    constexpr explicit GamaChunkPayload(
        const std::uint32_t image_gamma ) noexcept :
        PngChunkPayloadBase(),
        m_image_gamma( image_gamma ) {}
    explicit GamaChunkPayload( const std::span<const std::byte> & data );

    constexpr ~GamaChunkPayload() override = default;
    constexpr explicit GamaChunkPayload( const GamaChunkPayload & other ) =
        default;
    explicit GamaChunkPayload( GamaChunkPayload && other ) noexcept;

    constexpr GamaChunkPayload &
    operator=( const GamaChunkPayload & other ) = default;
    GamaChunkPayload & operator=( GamaChunkPayload && other ) noexcept;

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid() && m_image_gamma > 0;
    }
    constexpr void setInvalid() noexcept override {
        setBaseInvalid();
        m_image_gamma = 0;
    }

    [[nodiscard]] constexpr auto imageGamma() const noexcept {
        return m_image_gamma;
    }
};

} // namespace gAMA

namespace sRGB
{

class SrgbChunkPayload final : protected PngChunkPayloadBase<PngChunkType::sRGB>
{
    private:
    RenderingIntent m_rendering_intent;

    public:
    using PngChunkPayloadBase<PngChunkType::sRGB>::getChunkType;
    using PngChunkPayloadBase<PngChunkType::sRGB>::size;

    SrgbChunkPayload() = delete;
    constexpr explicit SrgbChunkPayload(
        const RenderingIntent rendering_intent ) noexcept :
        PngChunkPayloadBase(),
        m_rendering_intent( rendering_intent ) {}
    explicit SrgbChunkPayload( const std::span<const std::byte> & data );

    constexpr ~SrgbChunkPayload() override = default;
    constexpr explicit SrgbChunkPayload( const SrgbChunkPayload & other ) =
        default;
    explicit SrgbChunkPayload( SrgbChunkPayload && other ) noexcept;

    constexpr SrgbChunkPayload &
    operator=( const SrgbChunkPayload & other ) = default;
    SrgbChunkPayload & operator=( SrgbChunkPayload && other ) noexcept;

    [[nodiscard]] constexpr operator bool() const noexcept override {
        return isValid();
    }
    [[nodiscard]] constexpr bool isValid() const noexcept override {
        return isBaseValid() && PNG::sRGB::is_valid( m_rendering_intent );
    }
    constexpr void setInvalid() noexcept override {
        setBaseInvalid();
        m_rendering_intent = RenderingIntent::INVALID;
    }

    [[nodiscard]] constexpr auto renderingIntent() const noexcept {
        return m_rendering_intent;
    }
};

} // namespace sRGB

// Ancillary Chunks

} // namespace PNG
