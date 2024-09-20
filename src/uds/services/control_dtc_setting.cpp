#include "uds/services/control_dtc_setting.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool ControlDtcSettingService::handle(
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

    if (sub != 0x01 && sub != 0x02) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    if (session.current() == SessionType::Default) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    const uint8_t* record     = (req_len > 2) ? req + 2 : nullptr;
    size_t         record_len = (req_len > 2) ? req_len - 2 : 0;

    if (m_callback) {
        m_callback(static_cast<DtcSettingType>(sub), record, record_len, m_ctx);
    }

    resp[0]  = static_cast<uint8_t>(ServiceId::ControlDtcSetting) + 0x40;
    resp[1]  = sub;
    resp_len = 2;
    return !suppress;
}

} // namespace uds
