#include "uds/services/write_data_by_identifier.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool WriteDataByIdentifierService::register_did(const DataIdentifier& entry)
{
    if (m_count >= kMaxDataIdentifiers) return false;
    m_dids[m_count++] = entry;
    return true;
}

const DataIdentifier* WriteDataByIdentifierService::find_did(uint16_t did) const
{
    for (size_t i = 0; i < m_count; ++i)
        if (m_dids[i].did == did) return &m_dids[i];
    return nullptr;
}

bool WriteDataByIdentifierService::handle(
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
    const DataIdentifier* did = find_did(did_id);

    if (!did) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestOutOfRange);
        return true;
    }

    if (!did->writable || !did->write_fn) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SecurityAccessDenied);
        return true;
    }

    size_t data_len = req_len - 3;
    if (!did->write_fn(req + 3, data_len, did->ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::GeneralProgrammingFailure);
        return true;
    }

    resp[0] = static_cast<uint8_t>(ServiceId::WriteDataByIdentifier) + 0x40;
    resp[1] = static_cast<uint8_t>(did_id >> 8);
    resp[2] = static_cast<uint8_t>(did_id & 0xFF);
    resp_len = 3;
    return true;
}

} // namespace uds
