#include "uds/services/request_transfer_exit.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool RequestTransferExitService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    if (!m_dl.is_active()) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
        return true;
    }

    const uint8_t* param     = (req_len > 1) ? req + 1 : nullptr;
    size_t         param_len = (req_len > 1) ? req_len - 1 : 0;

    resp[0] = static_cast<uint8_t>(ServiceId::RequestTransferExit) + 0x40;
    size_t resp_param_len = resp_len - 1;

    if (m_callback && !m_callback(param, param_len, resp + 1, &resp_param_len, m_ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::GeneralProgrammingFailure);
        return true;
    }

    m_dl.reset();
    m_td.reset();

    resp_len = 1 + (m_callback ? resp_param_len : 0);
    return true;
}

} // namespace uds
