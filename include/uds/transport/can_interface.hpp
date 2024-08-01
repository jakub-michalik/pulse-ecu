#pragma once

#include <cstdint>
#include <cstddef>

namespace uds {
namespace transport {

struct CanFrame {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  dlc;
    bool     is_extended_id;
};

// Abstract CAN bus interface - implement for your specific hardware
class ICanInterface {
public:
    virtual ~ICanInterface() = default;

    // Send a CAN frame - returns true on success
    virtual bool send(const CanFrame& frame) = 0;

    // Receive a CAN frame - returns true if a frame was received
    virtual bool receive(CanFrame& frame) = 0;

    // Get current time in milliseconds (used for timeouts)
    virtual uint32_t get_tick_ms() = 0;
};

} // namespace transport
} // namespace uds
