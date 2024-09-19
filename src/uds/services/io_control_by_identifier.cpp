#include "uds/services/io_control_by_identifier.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool InputOutputControlByIdentifierService::register_io_control(const IoControlEntry& entry)
{
    if (m_count >= kMaxIoControls) return false;
    m_controls[m_count++] = entry;
    return true;
}

const IoControlEntry* InputOutputControlByIdentifierService::find(uint16_t did) const
{
    for (size_t i = 0; i < m_count; ++i)
        if (m_controls[i].did == did) return &m_controls[i];
    return nullptr;
}

bool InputOutputControlByIdentifierService::handle(
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

    uint16_t did_id = static_cast<uint16_t>(req[1] << 8) | req[2];
    uint8_t  action = req[3];

    const IoControlEntry* ctrl = find(did_id);
    if (!ctrl) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestOutOfRange);
        return true;
    }

    if (ctrl->required_session != SessionType::Default &&
        ctrl->required_session != session.current()) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    const uint8_t* opt     = (req_len > 4) ? req + 4 : nullptr;
    size_t         opt_len = (req_len > 4) ? req_len - 4 : 0;
    size_t         rec_len = resp_len - 4;

    resp[0] = static_cast<uint8_t>(ServiceId::InputOutputControlByIdentifier) + 0x40;
    resp[1] = static_cast<uint8_t>(did_id >> 8);
    resp[2] = static_cast<uint8_t>(did_id & 0xFF);
    resp[3] = action;

    if (!ctrl->control(static_cast<IoControlAction>(action),
                        opt, opt_len, resp + 4, &rec_len, ctrl->ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::ConditionsNotCorrect);
        return true;
    }

    resp_len = 4 + rec_len;
    return true;
}

} // namespace uds
