//
// Created by ben on 20/02/2022.
//

#ifndef JPEG_PROJECT_CRC_HPP
#define JPEG_PROJECT_CRC_HPP

#include <cstdint>

void GenCRCTable() noexcept;
uint32_t UpdateCRCTable(const uint32_t& crc, const unsigned char* buf, const int32_t& len) noexcept;
uint32_t CRC32(const unsigned char* buf, const uint32_t& _len) noexcept;

#endif //JPEG_PROJECT_CRC_HPP