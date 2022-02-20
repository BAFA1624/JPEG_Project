//
// Created by ben on 20/02/2022.
//

#include "../includes/CRC.hpp"

bool CRC_table_computed = false;
unsigned long CRC_table[256];

void GenCRCTable() noexcept
{
    if (!CRC_table_computed) {
        uint32_t c;

        for (int i = 0; i < 256; ++i) {
            c = static_cast<uint32_t>(i);
            for (int j = 0; j < 8; ++j) {
                    if (c & 1)
                        c = 0xedb88320L ^ (c >> 1);
                    else
                        c >>= 1;
            }
            CRC_table[i] = c;
        }

        CRC_table_computed = true;
    }
}

uint32_t UpdateCRCTable(const uint32_t& crc, const unsigned char* buf, const int32_t& len) noexcept
{
    uint32_t c = crc;

    if (!CRC_table_computed)
        GenCRCTable();
    for (int i = 0; i < static_cast<int32_t>(len); ++i)
        c = CRC_table[(c ^ buf[i]) & 0xff] ^ (c >> 8);

    return c;
}

uint32_t CRC32(const unsigned char* _buf, const uint32_t& _len) noexcept
{
    return UpdateCRCTable(0xffffffffL, _buf, _len) ^ 0xffffffffL;
}
