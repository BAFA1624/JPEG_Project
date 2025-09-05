#include "png/png_types.hpp"

#include "common/crc.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

namespace PNG
{

constexpr bool
isValid( const PngChunkType png_chunk_type ) {
    return std::ranges::contains( valid_png_chunk_t, png_chunk_type );
}

std::ostream &
operator<<( std::ostream & out_stream, const PngChunkType chunk_type ) {
    const std::uint32_t type_copy{
        convert_endian<std::endian::little, std::endian::big>(
            static_cast<std::uint32_t>( chunk_type ) )
    };
    const auto * type_ptr = reinterpret_cast<const char *>( &type_copy );
    const auto   type_string{ std::string{ type_ptr, 4 } };
    out_stream << type_string;

    switch ( chunk_type ) {
    case PngChunkType::INVALID: {
        out_stream << "INVALID (internal)";
    } break;
    case PngChunkType::IHDR: {
        out_stream << " (critical)";
        break;
    }
    case PngChunkType::PLTE: {
        out_stream << " (critical)";
        break;
    }
    case PngChunkType::IDAT: {
        out_stream << " (critical)";
        break;
    }
    case PngChunkType::IEND: {
        out_stream << " (critical)";
        break;
    }
    case PngChunkType::bKGD: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::cHRM: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::dSIG: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::eXIF: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::gAMA: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::hIST: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::iCCP: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::iTXt: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::pHYs: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::sBIT: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::sPLT: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::sRGB: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::sTER: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::tEXt: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::tIME: {
        out_stream << " (ancillary)";
        break;
    }
    case PngChunkType::tRNS: {
        out_stream << " (ancillary)";
        break;
    }
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

constexpr bool
isValid( const BitDepth bit_depth ) {
    return std::ranges::contains( valid_bit_depths, bit_depth );
}

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

constexpr bool
isValid( const ColourType colour_type ) {
    return std::ranges::contains( valid_colour_types, colour_type );
}

constexpr bool
isValid( const ColourType colour_type, const BitDepth bit_depth ) {
    bool is_valid{ false };
    switch ( colour_type ) {
    case ColourType::GREYSCALE: {
        is_valid = PNG::IHDR::isValid( bit_depth );
    } break;
    case ColourType::INDEXED_COLOUR: {
        constexpr std::array<BitDepth, 4> valid_bit_depths{ 1, 2, 4, 8 };
        is_valid = PNG::IHDR::isValid( bit_depth )
                   && std::ranges::contains( valid_bit_depths, bit_depth );
    } break;
    case ColourType::TRUE_COLOUR: [[fallthrough]];
    case ColourType::GREYSCALE_ALPHA: [[fallthrough]];
    case ColourType::TRUE_COLOUR_ALPHA: {
        constexpr std::array<BitDepth, 3> valid_bit_depths{ 8, 16 };
        is_valid = PNG::IHDR::isValid( bit_depth )
                   && std::ranges::contains( valid_bit_depths, bit_depth );
    } break;
    case ColourType::INVALID: [[fallthrough]];
    default: is_valid = false;
    }

    return is_valid;
}

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

constexpr bool
isValid( const CompressionMethod compression_method ) {
    return compression_method == CompressionMethod::COMPRESSION_METHOD_0;
}

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

constexpr bool
isValid( const FilterMethod filter_method ) {
    return filter_method == FilterMethod::FILTER_METHOD_0;
}

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

constexpr bool
isValid( const InterlaceMethod interlace_method ) {
    return interlace_method == InterlaceMethod::NO_INTERLACE
           || interlace_method == InterlaceMethod::ADAM_7;
}

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
                                      palette.red, palette.green,
                                      palette.blue );
}

constexpr std::vector<Palette>
bytesToPalettes( const std::span<const std::byte> & data ) {
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

} // namespace PNG
