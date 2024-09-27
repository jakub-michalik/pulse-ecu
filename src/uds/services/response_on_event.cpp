#include "uds/services/response_on_event.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool ResponseOnEventService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    if (req_len < 4) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    uint8_t sub        = req[1] & 0x7F;
    bool    suppress   = (req[1] & 0x80) != 0;
    uint8_t evt_window = req[2];

    switch (static_cast<RoeSubFunc>(sub)) {
    case RoeSubFunc::StopResponseOnEvent:
        m_active = false;
        break;
    case RoeSubFunc::StartResponseOnEvent:
        m_active     = true;
        m_event_type = req[3];
        m_event_window = evt_window;
        break;
    case RoeSubFunc::ClearResponseOnEvent:
        m_active     = false;
        m_event_type = 0;
        break;
    default:
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    resp[0]  = static_cast<uint8_t>(ServiceId::ResponseOnEvent) + 0x40;
    resp[1]  = sub;
    resp[2]  = evt_window;
    resp[3]  = m_event_type;
    resp_len = 4;
    return !suppress;
}

} // namespace uds
