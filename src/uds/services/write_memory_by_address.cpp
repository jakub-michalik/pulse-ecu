#include "uds/services/write_memory_by_address.hpp"
#include "uds/uds_session.hpp"

namespace uds {

bool WriteMemoryByAddressService::handle(
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

    size_t data_start = 2 + addr_len + mem_len;
    if (req_len < data_start + mem_size) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    if (!m_callback || !m_callback(address, req + data_start, mem_size, m_ctx)) {
        write_nrc(resp, resp_len, service_id(), NrcCode::GeneralProgrammingFailure);
        return true;
    }

    resp[0] = static_cast<uint8_t>(ServiceId::WriteMemoryByAddress) + 0x40;
    resp[1] = addr_len_fmt;
    for (uint8_t i = 0; i < addr_len; ++i)
        resp[2 + i] = req[2 + i];
    resp_len = 2 + addr_len;
    return true;
}

} // namespace uds
