#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

enum class RoeSubFunc : uint8_t {
    StopResponseOnEvent  = 0x00,
    OnDtcStatusChange    = 0x01,
    OnTimerInterrupt     = 0x02,
    OnChangeOfDataId     = 0x03,
    StartResponseOnEvent = 0x05,
    ClearResponseOnEvent = 0x06,
};

class ResponseOnEventService : public IService {
public:
    ResponseOnEventService() : m_active(false), m_event_type(0) {}

    ServiceId service_id() const override { return ServiceId::ResponseOnEvent; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

    bool is_active() const { return m_active; }

private:
    bool    m_active;
    uint8_t m_event_type;
    uint8_t m_event_window;
    uint8_t m_service_to_respond;
};

} // namespace uds
