#include "uds/services/communication_control.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool CommunicationControlService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    if (req_len < 3) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    uint8_t sub       = req[1] & 0x7F;
    bool    suppress  = (req[1] & 0x80) != 0;
    uint8_t comm_type = req[2];

    if (sub > 0x03) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    if (session.current() == SessionType::Default) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    if (m_callback) {
        m_callback(static_cast<CommControlSubFunc>(sub),
                   static_cast<CommunicationType>(comm_type & 0x0F),
                   m_ctx);
    }

    resp[0]  = static_cast<uint8_t>(ServiceId::CommunicationControl) + 0x40;
    resp[1]  = sub;
    resp_len = 2;
    return !suppress;
}

} // namespace uds
