#include "png_types.hpp"

constexpr PNGChunk::PNGChunk() :
    len( 0 ), type( png_chunk_t::INVALID ), data( std::nullptr ), crc() {}
