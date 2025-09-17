#pragma once // PNG_CHUNK_HPP

#include "common/crc.hpp"
#include "png/png_chunk_payload.hpp"

#include <memory>

namespace PNG
{

class PngChunk
{
    private:
    // Pointer to held payload
    std::unique_ptr<PngChunkPayloadBase> m_payload;
    // Cyclic Redundancy Check for this chunk
    CRC::crc_t m_crc;

    protected:
    public:
};

} // namespace PNG
