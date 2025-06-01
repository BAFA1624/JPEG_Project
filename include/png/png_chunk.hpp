#pragma once // PNG_CHUNK_HPP

#include "png/png_chunk_base.hpp"

#include <memory>

namespace PNG
{

class PngChunkIHDR : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::IHDR };
};

class PngChunkPLTE : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::PLTE };
};

class PngChunkIDAT : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::IDAT };
};

class PngChunkIEND : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::IEND };
};

} // namespace PNG
