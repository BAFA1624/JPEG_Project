#include "png/png_types.hpp"

#include "common/crc.hpp"

#include <cassert>

namespace PNG
{

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

} // namespace PNG
