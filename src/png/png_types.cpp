#include "png/png_types.hpp"

#include <cassert>

namespace PNG
{

std::ostream &
operator<<( std::ostream & out_stream, const png_chunk_t chunk_type ) {
    switch ( chunk_type ) {
    case png_chunk_t::INVALID: out_stream << "INVALID (internal)"; break;
    case png_chunk_t::IHDR: out_stream << "IHDR (critical)"; break;
    case png_chunk_t::PLTE: out_stream << "PLTE (critical)"; break;
    case png_chunk_t::IDAT: out_stream << "IDAT (critical)"; break;
    case png_chunk_t::IEND: out_stream << "IEND (critical)"; break;
    case png_chunk_t::bKGD: out_stream << "bKGD (ancillary)"; break;
    case png_chunk_t::cHRM: out_stream << "cHRM (ancillary)"; break;
    case png_chunk_t::dSIG: out_stream << "dSIG (ancillary)"; break;
    case png_chunk_t::eXIF: out_stream << "eXIF (ancillary)"; break;
    case png_chunk_t::gAMA: out_stream << "gAMA (ancillary)"; break;
    case png_chunk_t::hIST: out_stream << "hIST (ancillary)"; break;
    case png_chunk_t::iCCP: out_stream << "iCCP (ancillary)"; break;
    case png_chunk_t::iTXt: out_stream << "iTXt (ancillary)"; break;
    case png_chunk_t::pHYs: out_stream << "pHYs (ancillary)"; break;
    case png_chunk_t::sBIT: out_stream << "sBIT (ancillary)"; break;
    case png_chunk_t::sPLT: out_stream << "sPLT (ancillary)"; break;
    case png_chunk_t::sRGB: out_stream << "sRGB (ancillary)"; break;
    case png_chunk_t::sTER: out_stream << "sTER (ancillary)"; break;
    case png_chunk_t::tEXt: out_stream << "tEXt (ancillary)"; break;
    case png_chunk_t::tIME: out_stream << "tIME (ancillary)"; break;
    case png_chunk_t::tRNS: out_stream << "tRNS (ancillary)"; break;
    case png_chunk_t::zTXt: out_stream << "zTXt (ancillary)"; break;
    default: out_stream << "default"; break;
    }

    return out_stream;
}

constexpr PNGChunk::PNGChunk() noexcept :
    block_size( 0 ),
    type( png_chunk_t::INVALID ),
    block_ptr( nullptr ),
    crc( 0 ) {}

constexpr PNGChunk::PNGChunk( const std::uint32_t     _block_size,
                              const png_chunk_t       _type,
                              const char * const      _source_ptr,
                              const std::bitset<32> & _crc ) noexcept :
    block_size( _block_size ),
    type( _type ),
    block_ptr( nullptr ),
    crc( _crc ) {
    block_ptr = std::make_unique<char *>( new char[block_size] );
    // TODO: Handle more gracefully
    assert( block_ptr );

    std::copy_n( _source_ptr, block_size, *block_ptr );
}

constexpr PNGChunk::PNGChunk( const std::uint32_t     _block_size,
                              const png_chunk_t       _type,
                              std::unique_ptr<char *> _source_ptr,
                              const std::bitset<32> & _crc ) noexcept :
    block_size( _block_size ),
    type( _type ),
    block_ptr( std::move( _source_ptr ) ),
    crc( _crc ) {}

constexpr PNGChunk::PNGChunk( PNGChunk && rhs ) noexcept :
    block_size( rhs.block_size ),
    type( rhs.type ),
    block_ptr( std::move( rhs.block_ptr ) ),
    crc( rhs.crc ) {
    // Invalidate rhs block
    rhs.block_size = 0;
    rhs.type = png_chunk_t::INVALID;
    rhs.block_ptr = nullptr;
    rhs.crc = 0;
}
constexpr PNGChunk &
PNGChunk::operator=( PNGChunk && rhs ) noexcept {
    if ( this != &rhs ) {
        block_size = rhs.block_size;
        type = rhs.type;
        block_ptr = std::move( rhs.block_ptr );
        crc = rhs.crc;

        rhs.block_size = 0;
        rhs.type = png_chunk_t::INVALID;
        rhs.block_ptr = nullptr;
        rhs.crc = 0;
    }

    return *this;
}

constexpr PNGChunk
PNGChunk::deep_copy() const {
    auto data_copy = std::make_unique<char *>( new char[block_size] );

    // TODO: Handle more gracefully
    assert( *data_copy );

    std::copy_n( *block_ptr, block_size, *data_copy );

    return PNGChunk{ block_size, type, std::move( data_copy ), crc };
}

constexpr bool
PNGChunk::is_valid() const noexcept {
    return type != png_chunk_t::INVALID;
}

} // namespace PNG
