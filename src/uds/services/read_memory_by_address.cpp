#include "uds/services/read_memory_by_address.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool ReadMemoryByAddressService::handle(
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

    uint8_t addr_len_fmt = req[1];
    uint8_t addr_len     = addr_len_fmt & 0x0F;
    uint8_t mem_len      = (addr_len_fmt >> 4) & 0x0F;

    if (addr_len == 0 || mem_len == 0 ||
        req_len < static_cast<size_t>(2 + addr_len + mem_len)) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    uint32_t address = 0;
    for (uint8_t i = 0; i < addr_len; ++i)
        address = (address << 8) | req[2 + i];

    size_t mem_size = 0;
    for (uint8_t i = 0; i < mem_len; ++i)
        mem_size = (mem_size << 8) | req[2 + addr_len + i];

    if (mem_size == 0 || mem_size > resp_len - 1) {
        write_nrc(resp, resp_len, service_id(), NrcCode::ResponseTooLong);
        return true;
    }

    resp[0] = static_cast<uint8_t>(ServiceId::ReadMemoryByAddress) + 0x40;

    if (!m_callback) {
        write_nrc(resp, resp_len, service_id(), NrcCode::ConditionsNotCorrect);
        return true;
    }

    size_t read = m_callback(address, resp + 1, mem_size, m_ctx);
    if (read == 0) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequestOutOfRange);
        return true;
    }

    resp_len = 1 + read;
    return true;
}

} // namespace uds
