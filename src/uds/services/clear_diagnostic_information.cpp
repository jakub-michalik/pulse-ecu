#include "uds/services/clear_diagnostic_information.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool ClearDiagnosticInformationService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    // Request: SID(1) + GroupOfDTC(3)
    if (req_len < 4) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    uint32_t group = static_cast<uint32_t>(req[1] << 16) |
                     static_cast<uint32_t>(req[2] << 8)  |
                     static_cast<uint32_t>(req[3]);

    if (group == 0xFFFFFF) {
        // Clear all
        m_dtc_manager.clear_all();
    } else {
        m_dtc_manager.clear_group(group);
    }

    resp[0]  = static_cast<uint8_t>(ServiceId::ClearDiagnosticInformation) + 0x40;
    resp_len = 1;
    return true;
}

} // namespace uds
