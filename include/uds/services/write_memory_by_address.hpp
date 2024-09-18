#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

// Return true if write was successful
using WriteMemoryCallback = bool (*)(uint32_t address,
                                     const uint8_t* data,
                                     size_t len,
                                     void* ctx);

class WriteMemoryByAddressService : public IService {
public:
    WriteMemoryByAddressService(WriteMemoryCallback cb, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx) {}

    ServiceId service_id() const override { return ServiceId::WriteMemoryByAddress; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    WriteMemoryCallback m_callback;
    void*               m_ctx;
};

} // namespace uds
