#pragma once // PNG_CHUNK_HPP

#include "common/crc.hpp"

namespace PNG
{

class PngChunk
{
    private:
    // Cyclic Redundancy Check for this chunk
    CRC::crc_t m_crc;

    protected:
    public:
};

} // namespace PNG
