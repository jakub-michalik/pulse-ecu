#pragma once

#include <cstdint>
#include <cstddef>

namespace uds {
namespace utils {

// CRC-32 (ISO 3309 / Ethernet polynomial 0xEDB88320)
uint32_t crc32(const uint8_t* data, size_t len, uint32_t crc_init = 0xFFFFFFFFu);

// CRC-16 CCITT (polynomial 0x1021)
uint16_t crc16_ccitt(const uint8_t* data, size_t len, uint16_t crc_init = 0xFFFFu);

// Simple checksum (byte sum)
uint8_t checksum8(const uint8_t* data, size_t len);

} // namespace utils
} // namespace uds
