#include "uds/services/read_data_by_identifier.hpp"
#include "uds/uds_session.hpp"
#include <cstring>

namespace uds {

bool ReadDataByIdentifierService::register_did(const DataIdentifier& entry)
{
    if (m_count >= kMaxDataIdentifiers) return false;
    m_dids[m_count++] = entry;
    return true;
}

const DataIdentifier* ReadDataByIdentifierService::find_did(uint16_t did) const
{
    for (size_t i = 0; i < m_count; ++i)
        if (m_dids[i].did == did) return &m_dids[i];
    return nullptr;
}

bool ReadDataByIdentifierService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    // Need at least SID + one DID (2 bytes)
    if (req_len < 3 || (req_len - 1) % 2 != 0) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    size_t num_dids = (req_len - 1) / 2;

    resp[0] = static_cast<uint8_t>(ServiceId::ReadDataByIdentifier) + 0x40;
    size_t pos = 1;

    for (size_t i = 0; i < num_dids; ++i) {
        uint16_t did_id = static_cast<uint16_t>(
            (static_cast<uint16_t>(req[1 + i * 2]) << 8) | req[2 + i * 2]);

        const DataIdentifier* did = find_did(did_id);
        if (!did) {
            // Per ISO 14229-1: requestOutOfRange for unknown DID
            write_nrc(resp, resp_len, service_id(), NrcCode::RequestOutOfRange);
            return true;
        }

        if (!did->read_fn) {
            // DID exists but read not supported
            write_nrc(resp, resp_len, service_id(), NrcCode::ConditionsNotCorrect);
            return true;
        }

        if (pos + 2 > resp_len - 1) {
            write_nrc(resp, resp_len, service_id(), NrcCode::ResponseTooLong);
            return true;
        }

        resp[pos++] = static_cast<uint8_t>(did_id >> 8);
        resp[pos++] = static_cast<uint8_t>(did_id & 0xFF);

        size_t avail   = resp_len - pos;
        size_t written = did->read_fn(resp + pos, avail, did->ctx);
        if (written == 0) {
            write_nrc(resp, resp_len, service_id(), NrcCode::ConditionsNotCorrect);
            return true;
        }
        pos += written;
    }

    resp_len = pos;
    return true;
}

} // namespace uds
