#pragma once

#include "service_base.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

enum class RoutineSubFunc : uint8_t {
    StartRoutine  = 0x01,
    StopRoutine   = 0x02,
    RequestResults = 0x03,
};

static constexpr size_t kMaxRoutines = 32;

struct RoutineEntry {
    uint16_t id;

    // Start routine - return true on success
    bool (*start)(const uint8_t* opt_record, size_t opt_len,
                  uint8_t* resp_record,  size_t* resp_len,
                  void* ctx);

    // Stop routine - may be null if stop not supported
    bool (*stop)(const uint8_t* opt_record, size_t opt_len,
                 uint8_t* resp_record,  size_t* resp_len,
                 void* ctx);

    // Get results - may be null
    bool (*results)(uint8_t* resp_record, size_t* resp_len, void* ctx);

    // Session required to run this routine
    SessionType required_session;
    void* ctx;
};

class RoutineControlService : public IService {
public:
    RoutineControlService() : m_count(0) {}

    ServiceId service_id() const override { return ServiceId::RoutineControl; }

    bool register_routine(const RoutineEntry& entry);

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    const RoutineEntry* find_routine(uint16_t id) const;

    RoutineEntry m_routines[kMaxRoutines];
    size_t       m_count;
};

} // namespace uds
