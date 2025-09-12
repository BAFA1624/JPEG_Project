#pragma once

#include "png/png_chunk_payload.hpp"

namespace PNG
{

bool test_png_chunk_payload_base();

const auto test_functions = std::vector{ test_png_chunk_payload_base };

} // namespace PNG
