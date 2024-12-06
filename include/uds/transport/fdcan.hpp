#pragma once

#include "isotp.hpp"

namespace uds {
namespace transport {

// Extended ISO-TP configuration for FDCAN
// CAN FD allows up to 64-byte frames, which changes SF/FF sizes
struct FdIsoTpConfig : IsoTpConfig {
    bool     fd_enabled;     // Use CAN FD frames (>8 bytes)
    uint8_t  sf_max_len;     // Max SF payload (7 classic, 62 FD)
    uint8_t  ff_min_len;     // Min FF total length (8 classic, 63 FD)
};

// CAN FD constants
static constexpr size_t kFdMaxPayload = 4095;  // ISO-TP max with 12-bit length
static constexpr uint8_t kFdSfMaxLen  = 62;    // CAN FD single frame max

} // namespace transport
} // namespace uds

// ISO-TP over CAN FD differences (ISO 15765-2:2016):
//
// Single Frame (SF):
//   - Classic: byte 0 = 0x0N (N = len, max 7)
//   - FD:      byte 0 = 0x00, byte 1 = N (N = 8..62)
//
// First Frame (FF):
//   - Classic: byte 0 = 0x1x, byte 1 = len (max 4095)
//   - FD long: byte 0 = 0x10, byte 1 = 0x00, bytes 2-5 = 32-bit len
//
// Flow Control, Consecutive Frame: same as classic CAN
