#include "uds/services/routine_control.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool RoutineControlService::register_routine(const RoutineEntry& entry)
{
    if (m_count >= kMaxRoutines) return false;
    m_routines[m_count++] = entry;
    return true;
}

const RoutineEntry* RoutineControlService::find_routine(uint16_t id) const
{
    for (size_t i = 0; i < m_count; ++i)
        if (m_routines[i].id == id) return &m_routines[i];
    return nullptr;
}

bool RoutineControlService::handle(
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

    uint8_t  sub     = req[1];
    uint16_t rtn_id  = static_cast<uint16_t>(req[2] << 8) | req[3];

    if (sub < 0x01 || sub > 0x03) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    const RoutineEntry* rtn = find_routine(rtn_id);
    if (!rtn) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestOutOfRange);
        return true;
    }

    if (rtn->required_session != SessionType::Default &&
        rtn->required_session != session.current()) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    // Build response header
    resp[0] = static_cast<uint8_t>(ServiceId::RoutineControl) + 0x40;
    resp[1] = sub;
    resp[2] = static_cast<uint8_t>(rtn_id >> 8);
    resp[3] = static_cast<uint8_t>(rtn_id & 0xFF);

    const uint8_t* opt     = (req_len > 4) ? req + 4 : nullptr;
    size_t         opt_len = (req_len > 4) ? req_len - 4 : 0;
    size_t         rec_max = resp_len - 4;
    size_t         rec_len = 0;
    bool           ok      = false;

    switch (static_cast<RoutineSubFunc>(sub)) {
    case RoutineSubFunc::StartRoutine:
        if (!rtn->start) {
            write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
            return true;
        }
        ok = rtn->start(opt, opt_len, resp + 4, &rec_len, rtn->ctx);
        break;

    case RoutineSubFunc::StopRoutine:
        if (!rtn->stop) {
            write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
            return true;
        }
        ok = rtn->stop(opt, opt_len, resp + 4, &rec_len, rtn->ctx);
        break;

    case RoutineSubFunc::RequestResults:
        if (!rtn->results) {
            write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
            return true;
        }
        ok = rtn->results(resp + 4, &rec_len, rtn->ctx);
        break;
    }

    if (!ok) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::FailurePreventsExecutionOfRequestedAction);
        return true;
    }

    resp_len = 4 + rec_len;
    return true;
}

} // namespace uds
