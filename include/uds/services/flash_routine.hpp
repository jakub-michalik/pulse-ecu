#pragma once

#include "routine_control.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {
namespace flash {

// Flash memory operation status
enum class FlashStatus {
    Ok,
    Busy,
    Error,
    AddressError,
    ProtectionError,
};

// Flash HAL interface - implement for your MCU
struct IFlash {
    virtual ~IFlash() = default;
    virtual FlashStatus erase_sector(uint32_t address) = 0;
    virtual FlashStatus write(uint32_t address, const uint8_t* data, size_t len) = 0;
    virtual FlashStatus verify(uint32_t address, const uint8_t* data, size_t len) = 0;
    virtual bool is_busy() = 0;
};

// Pre-built routine implementations for flash operations
RoutineEntry make_erase_routine(IFlash& flash, void* ctx = nullptr);
RoutineEntry make_verify_routine(IFlash& flash, void* ctx = nullptr);

struct EraseRoutineCtx {
    IFlash&  flash;
    uint32_t last_erased_addr;
    bool     done;
};

struct VerifyRoutineCtx {
    IFlash&  flash;
    uint32_t address;
    size_t   length;
    bool     ok;
};

} // namespace flash
} // namespace uds
