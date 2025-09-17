#pragma once

#include "png/png_chunk.hpp"

namespace PNG
{

// Critical Chunks
bool test_ihdr_chunk();
bool test_plte_chunk();
bool test_idat_chunk();
bool test_iend_chunk();

// Ancillary Chunks

const auto test_functions = std::vector{
    test_ihdr_chunk, test_plte_chunk, test_idat_chunk, test_iend_chunk
};

} // namespace PNG
