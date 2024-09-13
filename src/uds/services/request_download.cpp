#include "uds/services/request_download.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool RequestDownloadService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    // Minimum: SID(1) + dataFormatId(1) + addressAndLengthFormatId(1)
    if (req_len < 3) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    if (session.current() != SessionType::Programming) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    if (m_active) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
        return true;
    }

    uint8_t data_format  = req[1];
    uint8_t addr_len_fmt = req[2];

    uint8_t addr_len = (addr_len_fmt >> 0) & 0x0F;  // lower nibble
    uint8_t size_len = (addr_len_fmt >> 4) & 0x0F;  // upper nibble

    if (req_len < static_cast<size_t>(3 + addr_len + size_len)) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    DownloadRequest dl{};
    dl.compression = (data_format >> 4) & 0x0F;
    dl.encryption  = (data_format >> 0) & 0x0F;

    // Parse address (big endian)
    dl.address = 0;
    for (uint8_t i = 0; i < addr_len; ++i)
        dl.address = (dl.address << 8) | req[3 + i];

    // Parse length (big endian)
    dl.length = 0;
    for (uint8_t i = 0; i < size_len; ++i)
        dl.length = (dl.length << 8) | req[3 + addr_len + i];

    size_t max_block = 0;
    if (!m_callback || !m_callback(dl, max_block, m_ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::UploadDownloadNotAccepted);
        return true;
    }

    m_active = true;

    // Response: maxNumberOfBlockLen (length parameter = 2 bytes)
    uint8_t len_fmt = 0x20;  // 2 bytes for block length
    resp[0] = static_cast<uint8_t>(ServiceId::RequestDownload) + 0x40;
    resp[1] = len_fmt;
    resp[2] = static_cast<uint8_t>(max_block >> 8);
    resp[3] = static_cast<uint8_t>(max_block & 0xFF);
    resp_len = 4;
    return true;
}

} // namespace uds
