#pragma once

#include <cstdint>
#include <cstddef>

namespace uds {
namespace hal {

class ITimer {
public:
    virtual ~ITimer() = default;
    virtual uint32_t get_tick_ms() = 0;
};

class IRng {
public:
    virtual ~IRng() = default;
    virtual bool fill(uint8_t* buf, size_t len) = 0;
};

class INvs {
public:
    virtual ~INvs() = default;
    virtual bool read(uint32_t offset, uint8_t* buf, size_t len) = 0;
    virtual bool write(uint32_t offset, const uint8_t* buf, size_t len) = 0;
    virtual bool erase(uint32_t offset, size_t len) = 0;
};

} // namespace hal
} // namespace uds
