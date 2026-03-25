#include "png/png_chunk_payload_test.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <numeric>
#include <optional>
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

static_assert(
    std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::IHDR>> );
static_assert(
    !std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::IHDR>,
                             std::uint32_t> );
static_assert(
    !std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::PLTE>> );
static_assert(
    std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::PLTE>,
                            std::uint32_t> );
static_assert(
    !std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::IDAT>> );
static_assert(
    std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::IDAT>,
                            std::uint32_t> );
static_assert(
    std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::IEND>> );
static_assert(
    !std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::IEND>,
                             std::uint32_t> );
static_assert(
    std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::cHRM>> );
static_assert(
    !std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::cHRM>,
                             std::uint32_t> );
static_assert(
    std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::gAMA>> );
static_assert(
    !std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::gAMA>,
                             std::uint32_t> );
static_assert(
    std::is_default_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::sRGB>> );
static_assert(
    !std::is_constructible_v<PngChunkPayloadBaseWrapper<PngChunkType::sRGB>,
                             std::uint32_t> );

namespace
{

const auto png_signature =
    std::array{ std::byte{ 0x89 }, std::byte{ 0x50 }, std::byte{ 0x4E },
                std::byte{ 0x47 }, std::byte{ 0x0D }, std::byte{ 0x0A },
                std::byte{ 0x1A }, std::byte{ 0x0A } };

[[nodiscard]] std::filesystem::path
test_data_path( const std::string_view filename ) {
    return std::filesystem::current_path() / ".." / ".." / ".." / "tests"
           / "data" / filename;
}

[[nodiscard]] std::filesystem::path
repo_data_path( const std::string_view filename ) {
    return std::filesystem::current_path() / ".." / ".." / ".." / "data"
           / filename;
}

[[nodiscard]] std::vector<std::byte>
read_binary_file( const std::filesystem::path & path ) {
    std::ifstream file{ path, std::ios::binary | std::ios::in };
    if ( !file.is_open() ) {
        return {};
    }

    file.seekg( 0, std::ios_base::end );
    const auto file_size = file.tellg();
    if ( file_size < 0 ) {
        return {};
    }
    file.seekg( 0, std::ios_base::beg );

    std::vector<std::byte> file_data( static_cast<std::size_t>( file_size ) );
    file.read( reinterpret_cast<char *>( file_data.data() ), file_size );
    if ( !file.good() && !file.eof() ) {
        return {};
    }

    return file_data;
}

[[nodiscard]] std::optional<std::vector<std::byte>>
find_chunk_payload( const std::filesystem::path & path,
                    const PngChunkType            chunk_type,
                    const std::size_t occurrence = 0 ) {
    const auto file_bytes = read_binary_file( path );
    if ( file_bytes.size() < png_signature.size()
         || !std::ranges::equal( file_bytes | std::views::take( 8 ),
                                 png_signature ) ) {
        return std::nullopt;
    }

    std::size_t match_count{ 0 };
    std::size_t offset{ png_signature.size() };
    while ( offset + 12 <= file_bytes.size() ) {
        const auto chunk_size = span_to_integer<std::uint32_t, std::endian::big,
                                                std::endian::native>(
            std::span{ file_bytes.data() + offset, std::size_t{ 4 } } );
        const auto current_chunk_type = static_cast<PngChunkType>(
            span_to_integer<std::uint32_t, std::endian::big, std::endian::native>(
                std::span{ file_bytes.data() + offset + 4, std::size_t{ 4 } } ) );
        offset += 8;

        if ( offset + chunk_size + 4 > file_bytes.size() ) {
            return std::nullopt;
        }

        if ( current_chunk_type == chunk_type && match_count++ == occurrence ) {
            return std::vector<std::byte>{ file_bytes.begin()
                                               + static_cast<std::ptrdiff_t>( offset ),
                                           file_bytes.begin()
                                               + static_cast<std::ptrdiff_t>(
                                                   offset + chunk_size ) };
        }

        offset += chunk_size + 4;
    }

    return std::nullopt;
}

} // namespace

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
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( test_data_path( "indexed_palette.png" ),
                                PngChunkType::IHDR );
        if ( !payload_bytes ) {
            return false;
        }

        const IHDR::IhdrChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        return payload.isValid() && payload.getChunkType() == PngChunkType::IHDR
               && payload.size() == 13 && payload.getWidth() == 1
               && payload.getHeight() == 1;
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
                false>() ),
        TEST_INTERFACE::test_function( test_real_png, true )
        // Multiple Invalid Inputs:
    };

    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_plte_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<PLTE::PlteChunkPayload>( data );
    };
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( test_data_path( "indexed_palette.png" ),
                                PngChunkType::PLTE );
        if ( !payload_bytes ) {
            return false;
        }

        const PLTE::PlteChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        const auto palettes = payload.getPalettes();
        return payload.isValid() && payload.getChunkType() == PngChunkType::PLTE
               && payload.size() == 6 && payload.rChannel().size() == 2
               && payload.gChannel().size() == 2 && payload.bChannel().size() == 2
               && payload.rChannel()[0] == 0 && payload.gChannel()[0] == 0
               && payload.bChannel()[0] == 0 && payload.rChannel()[1] == 255
               && payload.gChannel()[1] == 255 && payload.bChannel()[1] == 255
               && palettes.size() == 2 && palettes[0].red == 0
               && palettes[0].green == 0 && palettes[0].blue == 0
               && palettes[1].red == 255 && palettes[1].green == 255
               && palettes[1].blue == 255;
    };
    const auto test_palette_round_trip = []() {
        const auto palettes = std::vector<PLTE::Palette>{
            { .red = 0, .green = 1, .blue = 2 },
            { .red = 3, .green = 4, .blue = 5 },
        };
        const PLTE::PlteChunkPayload payload{ palettes };
        const auto reconstructed = payload.getPalettes();
        return payload.isValid() && payload.size() == 6
               && payload.rChannel() == std::vector<PLTE::colour_t>{ 0, 3 }
               && payload.gChannel() == std::vector<PLTE::colour_t>{ 1, 4 }
               && payload.bChannel() == std::vector<PLTE::colour_t>{ 2, 5 }
               && reconstructed.size() == palettes.size()
               && reconstructed[0].red == palettes[0].red
               && reconstructed[0].green == palettes[0].green
               && reconstructed[0].blue == palettes[0].blue
               && reconstructed[1].red == palettes[1].red
               && reconstructed[1].green == palettes[1].green
               && reconstructed[1].blue == palettes[1].blue
               && payload[1].red == 3 && payload[1].green == 4
               && payload[1].blue == 5;
    };

    const auto valid_plte = std::array{ std::byte{ 0x00 }, std::byte{ 0x00 },
                                        std::byte{ 0x00 }, std::byte{ 0xFF },
                                        std::byte{ 0xFF }, std::byte{ 0xFF } };
    const auto invalid_plte = std::array{ std::byte{ 0x00 }, std::byte{ 0x00 },
                                          std::byte{ 0x00 }, std::byte{ 0xFF } };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_valid, true, std::span<const std::byte>{ valid_plte } ),
        TEST_INTERFACE::test_function(
            test_valid, false, std::span<const std::byte>{ invalid_plte } ),
        TEST_INTERFACE::test_function( test_palette_round_trip, true ),
        TEST_INTERFACE::test_function( test_real_png, true )
    };
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_idat_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<IDAT::IdatChunkPayload>( data );
    };
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( test_data_path( "indexed_palette.png" ),
                                PngChunkType::IDAT );
        if ( !payload_bytes ) {
            return false;
        }

        const IDAT::IdatChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        return payload.isValid() && payload.getChunkType() == PngChunkType::IDAT
               && payload.size() == payload_bytes->size()
               && payload.data().size() == payload_bytes->size();
    };

    const auto idat_data =
        std::array{ std::byte{ 0x78 }, std::byte{ 0x9C }, std::byte{ 0x63 } };
    const auto empty_data = std::array<std::byte, 0>{};

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_valid, true, std::span<const std::byte>{ idat_data } ),
        TEST_INTERFACE::test_function(
            test_valid, false, std::span<const std::byte>{ empty_data } ),
        TEST_INTERFACE::test_function( test_real_png, true )
    };
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_iend_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<IEND::IendChunkPayload>( data );
    };
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( test_data_path( "indexed_palette.png" ),
                                PngChunkType::IEND );
        if ( !payload_bytes ) {
            return false;
        }

        const IEND::IendChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        return payload.isValid() && payload.getChunkType() == PngChunkType::IEND
               && payload.size() == 0;
    };

    const auto empty_data = std::array<std::byte, 0>{};
    const auto non_empty_data = std::array{ std::byte{ 0x00 } };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_valid, true, std::span<const std::byte>{ empty_data } ),
        TEST_INTERFACE::test_function(
            test_valid, false, std::span<const std::byte>{ non_empty_data } ),
        TEST_INTERFACE::test_function( test_real_png, true )
    };
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_chrm_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<cHRM::ChrmChunkPayload>( data );
    };
    const auto test_move = []() {
        auto payload = cHRM::ChrmChunkPayload(
            31269, 32899, 63999, 33001, 30000, 60000, 15000, 5999 );
        const auto moved_payload = cHRM::ChrmChunkPayload( std::move( payload ) );
        return moved_payload.isValid() && !payload.isValid()
               && moved_payload.whitePointX() == 31269
               && moved_payload.blueY() == 5999;
    };
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( repo_data_path( "swirl.png" ),
                                PngChunkType::cHRM );
        if ( !payload_bytes ) {
            return false;
        }

        const cHRM::ChrmChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        return payload.isValid() && payload.getChunkType() == PngChunkType::cHRM
               && payload.size() == 32 && payload.whitePointX() == 31269
               && payload.whitePointY() == 32899 && payload.redX() == 63999
               && payload.redY() == 33001 && payload.greenX() == 30000
               && payload.greenY() == 60000 && payload.blueX() == 15000
               && payload.blueY() == 5999;
    };

    const auto chrm_data = std::array{
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x7A }, std::byte{ 0x25 },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x80 }, std::byte{ 0x83 },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0xF9 }, std::byte{ 0xFF },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x80 }, std::byte{ 0xE9 },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x75 }, std::byte{ 0x30 },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0xEA }, std::byte{ 0x60 },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x3A }, std::byte{ 0x98 },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x17 }, std::byte{ 0x6F }
    };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_valid, true, std::span<const std::byte>{ chrm_data } ),
        TEST_INTERFACE::test_function( test_move, true ),
        TEST_INTERFACE::test_function( test_real_png, true )
    };
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_gama_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<gAMA::GamaChunkPayload>( data );
    };
    const auto test_move = []() {
        auto payload = gAMA::GamaChunkPayload( 45455 );
        const auto moved_payload = gAMA::GamaChunkPayload( std::move( payload ) );
        return moved_payload.isValid() && !payload.isValid()
               && moved_payload.imageGamma() == 45455;
    };
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( test_data_path( "gamma_chunk.png" ),
                                PngChunkType::gAMA );
        if ( !payload_bytes ) {
            return false;
        }

        const gAMA::GamaChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        return payload.isValid() && payload.getChunkType() == PngChunkType::gAMA
               && payload.size() == 4 && payload.imageGamma() == 45455;
    };

    const auto valid_gama = std::array{
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0xB1 }, std::byte{ 0x8F }
    };
    const auto invalid_gama = std::array{
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x00 }
    };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_valid, true, std::span<const std::byte>{ valid_gama } ),
        TEST_INTERFACE::test_function(
            test_valid, false, std::span<const std::byte>{ invalid_gama } ),
        TEST_INTERFACE::test_function( test_move, true ),
        TEST_INTERFACE::test_function( test_real_png, true )
    };
    return TEST_INTERFACE::confirm_results( test_results );
}

bool
test_srgb_payload() {
    constexpr auto test_valid = []( const std::span<const std::byte> & data ) {
        return test_payload_valid<sRGB::SrgbChunkPayload>( data );
    };
    const auto test_move = []() {
        auto payload =
            sRGB::SrgbChunkPayload( sRGB::RenderingIntent::PERCEPTUAL );
        const auto moved_payload =
            sRGB::SrgbChunkPayload( std::move( payload ) );
        return moved_payload.isValid() && !payload.isValid()
               && moved_payload.renderingIntent()
                      == sRGB::RenderingIntent::PERCEPTUAL;
    };
    const auto test_real_png = []() {
        const auto payload_bytes =
            find_chunk_payload( test_data_path( "srgb_chunk.png" ),
                                PngChunkType::sRGB );
        if ( !payload_bytes ) {
            return false;
        }

        const sRGB::SrgbChunkPayload payload{ std::span<const std::byte>{
            payload_bytes->data(), payload_bytes->size() } };
        return payload.isValid() && payload.getChunkType() == PngChunkType::sRGB
               && payload.size() == 1
               && payload.renderingIntent() == sRGB::RenderingIntent::PERCEPTUAL;
    };

    const auto valid_srgb = std::array{ std::byte{ 0x00 } };
    const auto invalid_srgb = std::array{ std::byte{ 0x04 } };

    const auto test_results = std::vector<bool>{
        TEST_INTERFACE::test_function(
            test_valid, true, std::span<const std::byte>{ valid_srgb } ),
        TEST_INTERFACE::test_function(
            test_valid, false, std::span<const std::byte>{ invalid_srgb } ),
        TEST_INTERFACE::test_function( test_move, true ),
        TEST_INTERFACE::test_function( test_real_png, true )
    };
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
