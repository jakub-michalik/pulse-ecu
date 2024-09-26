#include "uds/services/link_control.hpp"
#include "uds/uds_session.hpp"

namespace uds {

// Standard baudrate table (ISO 14229-1 Table 201)
static const struct { uint8_t id; uint32_t bps; } kBaudrates[] = {
    { 0x01, 9600    },
    { 0x02, 19200   },
    { 0x03, 125000  },
    { 0x04, 250000  },
    { 0x05, 500000  },
    { 0x06, 1000000 },
};

uint32_t LinkControlService::resolve_fixed_baudrate(uint8_t id) const
{
    for (size_t i = 0; i < sizeof(kBaudrates)/sizeof(kBaudrates[0]); ++i)
        if (kBaudrates[i].id == id) return kBaudrates[i].bps;
    return 0;
}

bool LinkControlService::handle(
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

    uint8_t sub = req[1] & 0x7F;
    bool suppress = (req[1] & 0x80) != 0;

    if (session.current() == SessionType::Default) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    switch (static_cast<LinkControlSubFunc>(sub)) {
    case LinkControlSubFunc::VerifyBaudrateTransitionWithFixedBaudrate:
        if (req_len < 4) {
            write_nrc(resp, resp_len, service_id(),
                      NrcCode::IncorrectMessageLengthOrInvalidFormat);
            return true;
        }
        {
            uint32_t bps = resolve_fixed_baudrate(req[3]);
            if (bps == 0) {
                write_nrc(resp, resp_len, service_id(), NrcCode::RequestOutOfRange);
                return true;
            }
            m_pending_baudrate = bps;
            m_verified = true;
        }
        break;

    case LinkControlSubFunc::TransitionBaudrate:
        if (!m_verified) {
            write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
            return true;
        }
        if (m_callback) m_callback(m_pending_baudrate, m_ctx);
        m_verified = false;
        break;

    default:
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    resp[0]  = static_cast<uint8_t>(ServiceId::LinkControl) + 0x40;
    resp[1]  = sub;
    resp_len = 2;
    return !suppress;
}

} // namespace uds
