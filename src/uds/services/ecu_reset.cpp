#include "uds/services/ecu_reset.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool EcuResetService::handle(
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

    uint8_t sub = req[1] & 0x7F;  // mask suppress bit
    bool suppress = (req[1] & 0x80) != 0;

    if (sub < 0x01 || sub > 0x03) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    ResetType reset_type = static_cast<ResetType>(sub);

    // Build positive response
    resp[0] = static_cast<uint8_t>(ServiceId::EcuReset) + 0x40;
    resp[1] = sub;
    resp_len = 2;

    // Trigger reset after sending response
    if (m_callback) {
        m_callback(reset_type, m_user_data);
    }

    return !suppress;
}

} // namespace uds

// Warning suppression: ResetType parameter is intentionally passed through
// to the user callback even though the server doesn't use it after sending response.
