#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

enum class CommControlSubFunc : uint8_t {
    EnableRxAndTx           = 0x00,
    EnableRxAndDisableTx    = 0x01,
    DisableRxAndEnableTx    = 0x02,
    DisableRxAndTx          = 0x03,
};

enum class CommunicationType : uint8_t {
    NormalCommunication       = 0x01,
    NmCommunication           = 0x02,
    NetworkManagement         = 0x03,
};

using CommControlCallback = void (*)(CommControlSubFunc ctrl,
                                     CommunicationType  comm_type,
                                     void*              ctx);

class CommunicationControlService : public IService {
public:
    CommunicationControlService(CommControlCallback cb = nullptr, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx) {}

    ServiceId service_id() const override { return ServiceId::CommunicationControl; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    CommControlCallback m_callback;
    void*               m_ctx;
};

} // namespace uds
