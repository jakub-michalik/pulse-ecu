#include "uds/services/flash_routine.hpp"
#include <cstring>

namespace uds {
namespace flash {

static bool erase_start(const uint8_t* opt, size_t opt_len,
                         uint8_t* resp, size_t* resp_len, void* ctx)
{
    (void)opt; (void)opt_len;
    EraseRoutineCtx* c = reinterpret_cast<EraseRoutineCtx*>(ctx);
    if (!c) return false;

    if (c->flash.is_busy()) return false;

    uint32_t addr = 0;
    if (opt_len >= 4) {
        addr = static_cast<uint32_t>(opt[0] << 24) | (opt[1] << 16) |
               (opt[2] << 8) | opt[3];
    }

    FlashStatus s = c->flash.erase_sector(addr);
    if (s != FlashStatus::Ok) return false;

    c->last_erased_addr = addr;
    c->done = true;

    if (resp && resp_len) {
        *resp_len = 1;
        resp[0] = static_cast<uint8_t>(s);
    }
    return true;
}

static bool erase_results(uint8_t* resp, size_t* resp_len, void* ctx)
{
    EraseRoutineCtx* c = reinterpret_cast<EraseRoutineCtx*>(ctx);
    if (!c || !resp || !resp_len) return false;

    resp[0]   = c->done ? 0x10 : 0x00; // 0x10 = completed
    *resp_len = 1;
    return true;
}

RoutineEntry make_erase_routine(IFlash& flash, void* ctx)
{
    static EraseRoutineCtx erase_ctx { flash, 0, false };
    (void)ctx;

    RoutineEntry e{};
    e.id               = routine_id::kEraseFlashSector;
    e.start            = erase_start;
    e.stop             = nullptr;
    e.results          = erase_results;
    e.required_session = SessionType::Programming;
    e.ctx              = &erase_ctx;
    return e;
}

static bool verify_start(const uint8_t* opt, size_t opt_len,
                          uint8_t* resp, size_t* resp_len, void* ctx)
{
    VerifyRoutineCtx* c = reinterpret_cast<VerifyRoutineCtx*>(ctx);
    if (!c || opt_len < 8) return false;

    c->address = static_cast<uint32_t>(opt[0] << 24) | (opt[1] << 16) |
                 (opt[2] << 8) | opt[3];
    c->length  = static_cast<size_t>(opt[4] << 24) | (opt[5] << 16) |
                 (opt[6] << 8) | opt[7];

    // Verify will compare against expected data (passed in opt after addr+len)
    const uint8_t* expected = opt + 8;
    size_t         exp_len  = opt_len - 8;

    if (exp_len == 0) {
        c->ok = true;
    } else {
        FlashStatus s = c->flash.verify(c->address, expected, exp_len);
        c->ok = (s == FlashStatus::Ok);
    }

    if (resp && resp_len) {
        resp[0]   = c->ok ? 0x01 : 0x00;
        *resp_len = 1;
    }
    return true;
}

RoutineEntry make_verify_routine(IFlash& flash, void* ctx)
{
    static VerifyRoutineCtx verify_ctx { flash, 0, 0, false };
    (void)ctx;

    RoutineEntry e{};
    e.id               = routine_id::kVerifyFlash;
    e.start            = verify_start;
    e.stop             = nullptr;
    e.results          = nullptr;
    e.required_session = SessionType::Programming;
    e.ctx              = &verify_ctx;
    return e;
}

} // namespace flash
} // namespace uds
