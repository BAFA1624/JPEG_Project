#include "png/png_types.hpp"

#include <cassert>
#include <iomanip>
#include <string_view>

namespace PNG
{

std::ostream &
operator<<( std::ostream & out_stream, const png_chunk_t chunk_type ) {
    switch ( chunk_type ) {
    case png_chunk_t::INVALID: {
        out_stream << "INVALID (internal)";
    } break;
    case png_chunk_t::IHDR: {
        out_stream << "IHDR (critical)";
        break;
    }
    case png_chunk_t::PLTE: {
        out_stream << "PLTE (critical)";
        break;
    }
    case png_chunk_t::IDAT: {
        out_stream << "IDAT (critical)";
        break;
    }
    case png_chunk_t::IEND: {
        out_stream << "IEND (critical)";
        break;
    }
    case png_chunk_t::bKGD: {
        out_stream << "bKGD (ancillary)";
        break;
    }
    case png_chunk_t::cHRM: {
        out_stream << "cHRM (ancillary)";
        break;
    }
    case png_chunk_t::dSIG: {
        out_stream << "dSIG (ancillary)";
        break;
    }
    case png_chunk_t::eXIF: {
        out_stream << "eXIF (ancillary)";
        break;
    }
    case png_chunk_t::gAMA: {
        out_stream << "gAMA (ancillary)";
        break;
    }
    case png_chunk_t::hIST: {
        out_stream << "hIST (ancillary)";
        break;
    }
    case png_chunk_t::iCCP: {
        out_stream << "iCCP (ancillary)";
        break;
    }
    case png_chunk_t::iTXt: {
        out_stream << "iTXt (ancillary)";
        break;
    }
    case png_chunk_t::pHYs: {
        out_stream << "pHYs (ancillary)";
        break;
    }
    case png_chunk_t::sBIT: {
        out_stream << "sBIT (ancillary)";
        break;
    }
    case png_chunk_t::sPLT: {
        out_stream << "sPLT (ancillary)";
        break;
    }
    case png_chunk_t::sRGB: {
        out_stream << "sRGB (ancillary)";
        break;
    }
    case png_chunk_t::sTER: {
        out_stream << "sTER (ancillary)";
        break;
    }
    case png_chunk_t::tEXt: {
        out_stream << "tEXt (ancillary)";
        break;
    }
    case png_chunk_t::tIME: {
        out_stream << "tIME (ancillary)";
        break;
    }
    case png_chunk_t::tRNS: {
        out_stream << "tRNS (ancillary)";
        break;
    }
    case png_chunk_t::zTXt: {
        out_stream << "zTXt (ancillary)";
    } break;
        // clang-format off
    COLD default : {
        out_stream << "Unknown png_chunk_t received: "
               << static_cast<std::uint32_t>( chunk_type ) << ".";
        assert( false );
    } break;
        // clang-format on
    }

    return out_stream;
}

std::ostream &
operator<<( std::ostream & out_stream, const png_pixel_format_t pixel_format ) {
    switch ( pixel_format ) {
    case png_pixel_format_t::grayscale: {
        out_stream << "grayscale";
    } break;
    case png_pixel_format_t::truecolor: {
        out_stream << "truecolor";
    } break;
    case png_pixel_format_t::indexed: {
        out_stream << "indexed";
    } break;
    case png_pixel_format_t::alpha_grayscale: {
        out_stream << "alpha_grayscale";
    } break;
    case png_pixel_format_t::alpha_truecolor: {
        out_stream << "alpha_truecolor";
    } break;
        // clang-format off
    COLD default: {
        out_stream << "Unkown png_pixel_format_t received: " << static_cast<std::uint32_t>(pixel_format) << ".";
        } break;
        // clang-format on
    }
    return out_stream;
}

PNGChunk::PNGChunk( const std::uint32_t _data_size, const png_chunk_t _type,
                    char * const            _source_ptr,
                    const std::bitset<32> & _crc ) noexcept :
    data_size( _data_size ), type( _type ), block_ptr( nullptr ), crc( _crc ) {
    block_ptr = std::make_unique<char *>( _source_ptr );
    // TODO: Handle more gracefully
    assert( block_ptr.get() );
}

PNGChunk::PNGChunk( const std::uint32_t _data_size, const png_chunk_t _type,
                    std::unique_ptr<char *> _source_ptr,
                    const std::bitset<32> & _crc ) noexcept :
    data_size( _data_size ),
    type( _type ),
    block_ptr( std::move( _source_ptr ) ),
    crc( _crc ) {}

PNGChunk::PNGChunk( PNGChunk && rhs ) noexcept :
    data_size( rhs.data_size ),
    type( rhs.type ),
    block_ptr( std::unique_ptr<char *>( rhs.block_ptr.release() ) ),
    crc( rhs.crc ) {
    // Invalidate rhs block
    rhs.data_size = 0;
    rhs.type = png_chunk_t::INVALID;
    rhs.block_ptr = nullptr;
    rhs.crc = 0;
}

PNGChunk &
PNGChunk::operator=( PNGChunk && rhs ) noexcept {
    if ( this != &rhs ) {
        data_size = rhs.data_size;
        type = rhs.type;
        block_ptr = std::unique_ptr<char *>{ rhs.block_ptr.release() };
        crc = rhs.crc;

        rhs.data_size = 0;
        rhs.type = png_chunk_t::INVALID;
        rhs.block_ptr = nullptr;
        rhs.crc = 0;
    }

    return *this;
}

PNGChunk
PNGChunk::deep_copy() const {
    auto data_copy{ std::make_unique<char *>( new char[data_size] ) };

    std::copy_n( block_ptr.get(), data_size, data_copy.get() );

    return PNGChunk{ data_size, type, std::move( data_copy ), crc };
}

[[nodiscard]] std::ostream &
operator<<( std::ostream & out_stream, const PNGChunk & chunk ) {
    out_stream << "PNGChunk (" << chunk.type << "):\n"
               << "\t- Data Size: " << chunk.data_size << '\n'
               << "\t- CRC: " << chunk.crc << '\n'
               << "Block Data:\n--------\n"
               << *reinterpret_cast<std::uint64_t *>( *chunk.block_ptr )
               << "\n--------\n";
    return out_stream;
}

} // namespace PNG
