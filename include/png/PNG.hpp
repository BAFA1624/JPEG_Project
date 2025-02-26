#pragma once

#include "png_types.hpp"

#include <string_view>
#include <vector>

class PNG
{
    public:
    PNG() : png_chunks() {}

    private:
    PNGChunk process_chunk( const std::string_view chunk_data ) noexcept;

    std::vector<PNGChunk> png_chunks;
};
