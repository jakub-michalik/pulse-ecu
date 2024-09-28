#include "uds/services/request_upload.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool RequestUploadService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    if (req_len < 3) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    if (m_active) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
        return true;
    }

    uint8_t data_format  = req[1];
    uint8_t addr_len_fmt = req[2];
    uint8_t addr_len     = addr_len_fmt & 0x0F;
    uint8_t size_len     = (addr_len_fmt >> 4) & 0x0F;

    if (req_len < static_cast<size_t>(3 + addr_len + size_len)) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    UploadRequest ul{};
    ul.compression = (data_format >> 4) & 0x0F;
    ul.encryption  = data_format & 0x0F;
    ul.address     = 0;
    for (uint8_t i = 0; i < addr_len; ++i)
        ul.address = (ul.address << 8) | req[3 + i];
    ul.length = 0;
    for (uint8_t i = 0; i < size_len; ++i)
        ul.length = (ul.length << 8) | req[3 + addr_len + i];

    size_t max_block = 0;
    if (!m_callback || !m_callback(ul, max_block, m_ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::UploadDownloadNotAccepted);
        return true;
    }

    m_active = true;

    resp[0]  = static_cast<uint8_t>(ServiceId::RequestUpload) + 0x40;
    resp[1]  = 0x20;  // 2-byte block length
    resp[2]  = static_cast<uint8_t>(max_block >> 8);
    resp[3]  = static_cast<uint8_t>(max_block & 0xFF);
    resp_len = 4;
    return true;
}

} // namespace uds
