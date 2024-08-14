#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

enum class ResetType : uint8_t {
    HardReset         = 0x01,
    KeyOffOnReset     = 0x02,
    SoftReset         = 0x03,
};

// Callback type for ECU reset - user must provide reset implementation
using ResetCallback = void (*)(ResetType type, void* user_data);

class EcuResetService : public IService {
public:
    EcuResetService(ResetCallback cb, void* user_data = nullptr)
        : m_callback(cb), m_user_data(user_data) {}

    ServiceId service_id() const override { return ServiceId::EcuReset; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    ResetCallback m_callback;
    void*         m_user_data;
};

} // namespace uds
