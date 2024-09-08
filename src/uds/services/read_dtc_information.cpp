#include "uds/services/read_dtc_information.hpp"
#include "uds/uds_session.hpp"
#include <cstring>

namespace uds {

bool ReadDtcInformationService::handle(
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

    uint8_t sub = req[1];
    resp[0] = static_cast<uint8_t>(ServiceId::ReadDtcInformation) + 0x40;
    resp[1] = sub;
    size_t pos = 2;

    switch (static_cast<ReadDtcSubFunc>(sub)) {
    case ReadDtcSubFunc::ReportNumberOfDtcByStatusMask:
        if (req_len < 3) {
            write_nrc(resp, resp_len, service_id(),
                      NrcCode::IncorrectMessageLengthOrInvalidFormat);
            return true;
        }
        handle_report_count(req[2], resp, pos);
        break;

    case ReadDtcSubFunc::ReportDtcByStatusMask:
    case ReadDtcSubFunc::ReportSupportedDtc:
        if (req_len < 3) {
            write_nrc(resp, resp_len, service_id(),
                      NrcCode::IncorrectMessageLengthOrInvalidFormat);
            return true;
        }
        resp[pos++] = 0xFF; // DTCStatusAvailabilityMask
        handle_report_by_status(req[2], resp, pos, resp_len);
        break;

    default:
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    resp_len = pos;
    return true;
}

struct DtcWriteCtx {
    uint8_t* buf;
    size_t   pos;
    size_t   max;
    uint8_t  mask;
};

static bool write_dtc_cb(const dtc::DtcEntry& e, void* ctx)
{
    DtcWriteCtx* c = reinterpret_cast<DtcWriteCtx*>(ctx);
    if (c->pos + 4 > c->max) return false;

    c->buf[c->pos++] = static_cast<uint8_t>((e.dtc_code >> 16) & 0xFF);
    c->buf[c->pos++] = static_cast<uint8_t>((e.dtc_code >>  8) & 0xFF);
    c->buf[c->pos++] = static_cast<uint8_t>( e.dtc_code        & 0xFF);
    c->buf[c->pos++] = e.status & c->mask;
    return true;
}

void ReadDtcInformationService::handle_report_by_status(
    uint8_t mask, uint8_t* resp, size_t& pos, size_t max)
{
    DtcWriteCtx ctx{ resp, pos, max, mask };
    m_dtc_manager.for_each(mask, write_dtc_cb, &ctx);
    pos = ctx.pos;
}

void ReadDtcInformationService::handle_report_count(
    uint8_t mask, uint8_t* resp, size_t& pos)
{
    uint8_t avail_mask = 0xFF;
    resp[pos++] = avail_mask;
    resp[pos++] = 0x01; // DTC format identifier (ISO 14229-1)

    size_t cnt = m_dtc_manager.count_by_status(mask);
    resp[pos++] = static_cast<uint8_t>(cnt >> 8);
    resp[pos++] = static_cast<uint8_t>(cnt & 0xFF);
}

} // namespace uds
