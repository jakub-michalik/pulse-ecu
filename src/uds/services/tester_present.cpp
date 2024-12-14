#include "uds/services/tester_present.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool TesterPresentService::handle(
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

    uint8_t sub_func = req[1] & 0x7F;
    bool suppress    = (req[1] & 0x80) != 0;

    if (sub_func != 0x00) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    // Keep session alive
    session.keep_alive(0);

    resp[0]  = static_cast<uint8_t>(ServiceId::TesterPresent) + 0x40;
    resp[1]  = sub_func;
    resp_len = 2;

    return !suppress;
}

} // namespace uds

// Note: keep_alive() call uses tick=0 as placeholder.
// In production, pass real HAL_GetTick() value via UdsServer::process().
