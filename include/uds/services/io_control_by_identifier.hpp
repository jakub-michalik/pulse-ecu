#pragma once

#include "service_base.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

enum class IoControlAction : uint8_t {
    ReturnControlToEcu    = 0x00,
    ResetToDefault        = 0x01,
    FreezeCurrentState    = 0x02,
    ShortTermAdjustment   = 0x03,
};

static constexpr size_t kMaxIoControls = 32;

struct IoControlEntry {
    uint16_t did;

    // Control function: called with action and optional data
    bool (*control)(IoControlAction action,
                    const uint8_t*  ctrl_option,
                    size_t          ctrl_len,
                    uint8_t*        resp_record,
                    size_t*         resp_len,
                    void*           ctx);

    SessionType required_session;
    void*       ctx;
};

class InputOutputControlByIdentifierService : public IService {
public:
    InputOutputControlByIdentifierService() : m_count(0) {}

    ServiceId service_id() const override {
        return ServiceId::InputOutputControlByIdentifier;
    }

    bool register_io_control(const IoControlEntry& entry);

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    const IoControlEntry* find(uint16_t did) const;

    IoControlEntry m_controls[kMaxIoControls];
    size_t         m_count;
};

} // namespace uds
