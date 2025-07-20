#pragma once // PNG_CHUNK_HPP

#include "png/png_chunk_base.hpp"

namespace PNG
{

class PngChunkIHDR : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ PngChunkType::IHDR };
};

class PngChunkPLTE : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ PngChunkType::PLTE };
};

class PngChunkIDAT : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ PngChunkType::IDAT };
};

class PngChunkIEND : public PngChunkBase
{
    private:
    static constexpr auto m_chunk_type{ PngChunkType::IEND };
};

} // namespace PNG
