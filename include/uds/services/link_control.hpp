#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

enum class LinkControlSubFunc : uint8_t {
    VerifyBaudrateTransitionWithFixedBaudrate   = 0x01,
    VerifyBaudrateTransitionWithSpecificBaudrate = 0x02,
    TransitionBaudrate                          = 0x03,
};

struct BaudrateEntry {
    uint8_t  baudrate_id;   // fixed baudrate identifier
    uint32_t baudrate_bps;  // actual baud rate in bps
};

using LinkControlCallback = bool (*)(uint32_t new_baudrate_bps, void* ctx);

class LinkControlService : public IService {
public:
    LinkControlService(LinkControlCallback cb = nullptr, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx), m_verified(false), m_pending_baudrate(0) {}

    ServiceId service_id() const override { return ServiceId::LinkControl; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    uint32_t resolve_fixed_baudrate(uint8_t id) const;

    LinkControlCallback m_callback;
    void*               m_ctx;
    bool                m_verified;
    uint32_t            m_pending_baudrate;
};

} // namespace uds
