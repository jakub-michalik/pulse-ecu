#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

enum class DtcSettingType : uint8_t {
    On  = 0x01,
    Off = 0x02,
};

using DtcSettingCallback = void (*)(DtcSettingType setting,
                                    const uint8_t* record,
                                    size_t         record_len,
                                    void*          ctx);

class ControlDtcSettingService : public IService {
public:
    ControlDtcSettingService(DtcSettingCallback cb = nullptr, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx) {}

    ServiceId service_id() const override { return ServiceId::ControlDtcSetting; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    DtcSettingCallback m_callback;
    void*              m_ctx;
};

} // namespace uds
