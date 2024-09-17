#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

// Callback to read memory: return number of bytes actually read
using ReadMemoryCallback = size_t (*)(uint32_t address, uint8_t* buf,
                                      size_t len, void* ctx);

class ReadMemoryByAddressService : public IService {
public:
    ReadMemoryByAddressService(ReadMemoryCallback cb, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx) {}

    ServiceId service_id() const override { return ServiceId::ReadMemoryByAddress; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    ReadMemoryCallback m_callback;
    void*              m_ctx;
};

} // namespace uds
