#include "uds/services/transfer_data.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool TransferDataService::handle(
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

    if (!m_active) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
        return true;
    }

    uint8_t seq = req[1];

    if (seq != m_expected_seq) {
        write_nrc(resp, resp_len, service_id(), NrcCode::WrongBlockSequenceCounter);
        return true;
    }

    const uint8_t* data     = req + 2;
    size_t         data_len = req_len - 2;

    if (m_callback && !m_callback(seq, data, data_len, m_ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::GeneralProgrammingFailure);
        return true;
    }

    // Wrap sequence counter at 0xFF -> 0x00 -> 0x01
    m_expected_seq = (seq == 0xFF) ? 0x00 : seq + 1;

    resp[0]  = static_cast<uint8_t>(ServiceId::TransferData) + 0x40;
    resp[1]  = seq;
    resp_len = 2;
    return true;
}

} // namespace uds
