#pragma once

#include "service_base.hpp"
#include "uds/uds_session.hpp"

namespace uds {

// Callback invoked when session changes - user can perform actions on transition
using SessionChangeCallback = void (*)(SessionType old_session,
                                       SessionType new_session,
                                       void*       user_data);

class DiagnosticSessionControlService : public IService {
public:
    DiagnosticSessionControlService(
        SessionChangeCallback cb        = nullptr,
        void*                 user_data = nullptr)
        : m_callback(cb), m_user_data(user_data) {}

    ServiceId service_id() const override {
        return ServiceId::DiagnosticSessionControl;
    }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    SessionChangeCallback m_callback;
    void*                 m_user_data;
};

} // namespace uds
