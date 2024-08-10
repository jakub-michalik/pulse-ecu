#pragma once

#include "uds_types.hpp"
#include <cstdint>

namespace uds {

class UdsSession {
public:
    explicit UdsSession(const TimingConfig& timing = kDefaultTiming);

    // Current active session
    SessionType current() const { return m_session; }

    // Transition to new session - validates transition is allowed
    bool transition(SessionType new_session);

    // Reset to default session
    void reset();

    // Call periodically - handles S3 timeout
    void update(uint32_t tick_ms);

    // Reset S3 timer (call on any tester communication)
    void keep_alive(uint32_t tick_ms);

    // Returns true if session has timed out (S3 expired without TesterPresent)
    bool is_timed_out() const { return m_timed_out; }

    const TimingConfig& timing() const { return m_timing; }

private:
    SessionType  m_session;
    TimingConfig m_timing;
    uint32_t     m_s3_deadline;
    bool         m_timed_out;
};

} // namespace uds
