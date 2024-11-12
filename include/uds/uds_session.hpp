#pragma once

#include "uds_types.hpp"
#include <cstdint>

namespace uds {

using SessionTimeoutCallback = void (*)(void* ctx);

class UdsSession {
public:
    explicit UdsSession(const TimingConfig& timing = kDefaultTiming);

    SessionType current() const { return m_session; }

    bool transition(SessionType new_session);

    void reset();

    // Call periodically with current tick_ms
    void update(uint32_t tick_ms);

    // Reset S3 timer (call on any diagnostic communication)
    void keep_alive(uint32_t tick_ms);

    bool is_timed_out() const { return m_timed_out; }

    const TimingConfig& timing() const { return m_timing; }

    // Register a callback for session timeout notification
    void set_timeout_callback(SessionTimeoutCallback cb, void* ctx) {
        m_timeout_cb  = cb;
        m_timeout_ctx = ctx;
    }

private:
    SessionType  m_session;
    TimingConfig m_timing;
    uint32_t     m_s3_deadline;
    bool         m_timed_out;

    SessionTimeoutCallback m_timeout_cb;
    void*                  m_timeout_ctx;
};

} // namespace uds
