#include "uds/services/diagnostic_session_control.hpp"

namespace uds {

bool DiagnosticSessionControlService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    if (req_len < 2) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    uint8_t sub      = req[1] & 0x7F;
    bool    suppress = (req[1] & 0x80) != 0;

    if (sub < 0x01 || sub > 0x03) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    SessionType new_session = static_cast<SessionType>(sub);
    SessionType old_session = session.current();

    if (!session.transition(new_session)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::ConditionsNotCorrect);
        return true;
    }

    if (m_callback) {
        m_callback(old_session, new_session, m_user_data);
    }

    const TimingConfig& t = session.timing();

    resp[0] = static_cast<uint8_t>(ServiceId::DiagnosticSessionControl) + 0x40;
    resp[1] = sub;
    // P2 server max - high byte, low byte (in ms)
    resp[2] = static_cast<uint8_t>(t.p2_ms >> 8);
    resp[3] = static_cast<uint8_t>(t.p2_ms & 0xFF);
    // P2* server max - units of 10ms
    uint16_t p2_star_10ms = t.p2_star_ms / 10;
    resp[4] = static_cast<uint8_t>(p2_star_10ms >> 8);
    resp[5] = static_cast<uint8_t>(p2_star_10ms & 0xFF);
    resp_len = 6;

    return !suppress;
}

} // namespace uds
