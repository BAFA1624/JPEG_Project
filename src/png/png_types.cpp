#include "png/png_types.hpp"

#include "common/crc.hpp"

#include <cassert>
#include <ranges>

namespace PNG
{

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

} // namespace PLTE


std::ostream &
operator<<( std::ostream & out_stream, const PngChunkType chunk_type ) {
    const std::uint32_t type_copy{
        convert_endian<std::endian::little, std::endian::big>(
            static_cast<std::uint32_t>( chunk_type ) )
    };
    const auto type_string{ std::string{
        reinterpret_cast<const char *>( &type_copy ), 4 } };
    out_stream << type_string;

    switch ( chunk_type ) {
    case PngChunkType::INVALID: {
        out_stream << "INVALID (internal)";
    } break;
    case PngChunkType::IHDR: [[fallthrough]];
    case PngChunkType::PLTE: [[fallthrough]];
    case PngChunkType::IDAT: [[fallthrough]];
    case PngChunkType::IEND: {
        out_stream << " (critical)";
        break;
    }
    case PngChunkType::bKGD: [[fallthrough]];
    case PngChunkType::cHRM: [[fallthrough]];
    case PngChunkType::dSIG: [[fallthrough]];
    case PngChunkType::eXIF: [[fallthrough]];
    case PngChunkType::gAMA: [[fallthrough]];
    case PngChunkType::hIST: [[fallthrough]];
    case PngChunkType::iCCP: [[fallthrough]];
    case PngChunkType::iTXt: [[fallthrough]];
    case PngChunkType::pHYs: [[fallthrough]];
    case PngChunkType::sBIT: [[fallthrough]];
    case PngChunkType::sPLT: [[fallthrough]];
    case PngChunkType::sRGB: [[fallthrough]];
    case PngChunkType::sTER: [[fallthrough]];
    case PngChunkType::tEXt: [[fallthrough]];
    case PngChunkType::tIME: [[fallthrough]];
    case PngChunkType::tRNS: [[fallthrough]];
    case PngChunkType::zTXt: {
        out_stream << " (ancillary)";
    } break;
        // clang-format off
    COLD default : {
        out_stream << "Unknown PngChunkType received: "
               << static_cast<std::uint32_t>( chunk_type ) << ".";
        assert( false );
    } break;
        // clang-format on
    }

    return out_stream;
}

std::ostream &
operator<<( std::ostream & out_stream, const PngPixelFormat pixel_format ) {
    switch ( pixel_format ) {
    case PngPixelFormat::grayscale: {
        out_stream << "grayscale";
    } break;
    case PngPixelFormat::truecolor: {
        out_stream << "truecolor";
    } break;
    case PngPixelFormat::indexed: {
        out_stream << "indexed";
    } break;
    case PngPixelFormat::alpha_grayscale: {
        out_stream << "alpha_grayscale";
    } break;
    case PngPixelFormat::alpha_truecolor: {
        out_stream << "alpha_truecolor";
    } break;
        // clang-format off
    COLD default: {
        out_stream << "Unkown PngPixelFormat received: " << static_cast<std::uint32_t>(pixel_format) << ".";
        } break;
        // clang-format on
    }
    return out_stream;
}

namespace IHDR
{

// BitDepth

std::ostream &
operator<<( std::ostream & out_stream, const BitDepth bit_depth ) {
    switch ( static_cast<std::uint32_t>( bit_depth ) ) {
    case 1: [[fallthrough]];
    case 2: [[fallthrough]];
    case 4: [[fallthrough]];
    case 8: [[fallthrough]];
    case 16:
        return out_stream << std::format( "BitDepth ({})", bit_depth );
        // clang-format off
    COLD default:
        return out_stream << std::format( "INVALID BitDepth ({})", bit_depth );
        // clang-format on
    }
}

// ColourType

std::ostream &
operator<<( std::ostream & out_stream, const ColourType colour_type ) {
    switch ( colour_type ) {
    case ColourType::GREYSCALE: {
        return out_stream << std::format(
                   "GREYSCALE ({})",
                   static_cast<std::uint32_t>( colour_type ) );
    } break;
    case ColourType::TRUE_COLOUR: {
        return out_stream << std::format(
                   "TRUE_COLOUR ({})",
                   static_cast<std::uint32_t>( colour_type ) );
    } break;
    case ColourType::INDEXED_COLOUR: {
        return out_stream << std::format(
                   "INDEXED_COLOUR ({})",
                   static_cast<std::uint32_t>( colour_type ) );
    } break;
    case ColourType::GREYSCALE_ALPHA: {
        return out_stream << std::format(
                   "GREYSCALE_ALPHA ({})",
                   static_cast<std::uint32_t>( colour_type ) );
    } break;
    case ColourType::TRUE_COLOUR_ALPHA: {
        return out_stream << std::format(
                   "TRUE_COLOUR_ALPHA ({})",
                   static_cast<std::uint32_t>( colour_type ) );
    } break;
        // clang-format off
    COLD default: {
        return out_stream << std::format(
                   "INVALID ColourType ({})",
                   static_cast<std::uint32_t>( colour_type ) );
    }
        // clang-format on
    }
}

// CompressionMethod

std::ostream &
operator<<( std::ostream &          out_stream,
            const CompressionMethod compression_method ) {
    switch ( compression_method ) {
    case CompressionMethod::COMPRESSION_METHOD_0: {
        return out_stream << std::format(
                   "COMPRESSION_METHOD_0 ({})",
                   static_cast<std::uint32_t>( compression_method ) );
    } break;
        // clang-format off
    COLD default: {
        return out_stream << std::format(
                   "INVALID CompressionMethod ({})",
                   static_cast<std::uint32_t>( compression_method ) );
    }
        // clang-format on
    }
}

// FilterMethod

std::ostream &
operator<<( std::ostream & out_stream, const FilterMethod filter_method ) {
    switch ( filter_method ) {
    case FilterMethod::FILTER_METHOD_0: {
        return out_stream << std::format(
                   "FILTER_METHOD_0 ({})",
                   static_cast<std::uint32_t>( filter_method ) );
    } break;
        // clang-format off
    COLD default: {
        return out_stream << std::format(
                   "INVALID FilterMethod ({})",
                   static_cast<std::uint32_t>( filter_method ) );
    }
        // clang-format on
    }
}

// InterlaceMethod

std::ostream &
operator<<( std::ostream &        out_stream,
            const InterlaceMethod interlace_method ) {
    switch ( interlace_method ) {
    case InterlaceMethod::NO_INTERLACE: {
        return out_stream << std::format(
                   "NO_INTERLACE ({})",
                   static_cast<std::uint32_t>( interlace_method ) );
    } break;
    case InterlaceMethod::ADAM_7: {
        return out_stream << std::format(
                   "ADAM_7 ({})",
                   static_cast<std::uint32_t>( interlace_method ) );
    } break;
        // clang-format off
    COLD default: {
        return out_stream << std::format(
                   "INVALID InterlaceMethod ({})",
                   static_cast<std::uint32_t>( interlace_method ) );
    }
        // clang-format on
    }
}

} // namespace IHDR

namespace PLTE
{

std::ostream &
operator<<( std::ostream & out_stream, const Palette palette ) {
    return out_stream << std::format( "Palette( r: {}, g: {}, b: {} )",
                                      palette.red,
                                      palette.green,
                                      palette.blue );
}
} // namespace PLTE

} // namespace PNG
