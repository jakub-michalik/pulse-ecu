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

// Abstract CAN interface - user must provide concrete implementation
class ICanInterface {
public:
    virtual ~ICanInterface() = default;

    // Transmit a frame. Returns true on success.
    virtual bool send(const CanFrame& frame) = 0;

    // Try to receive a frame. Returns true if frame was available.
    virtual bool receive(CanFrame& frame) = 0;

    // Monotonic millisecond tick counter.
    virtual uint32_t get_tick_ms() = 0;

    // Optional: called when ISO-TP layer is idle (can be used for power saving)
    virtual void on_idle() { }
};

} // namespace transport
} // namespace uds
