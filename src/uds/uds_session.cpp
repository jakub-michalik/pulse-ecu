#include "uds/uds_session.hpp"

namespace uds {

UdsSession::UdsSession(const TimingConfig& timing)
    : m_session(SessionType::Default)
    , m_timing(timing)
    , m_s3_deadline(0)
    , m_timed_out(false)
{
}

bool UdsSession::transition(SessionType new_session)
{
    // From default session, any session change is allowed
    // From non-default sessions, only same or default is allowed without special conditions
    m_session   = new_session;
    m_timed_out = false;
    return true;
}

void UdsSession::reset()
{
    m_session   = SessionType::Default;
    m_timed_out = false;
}

void UdsSession::update(uint32_t tick_ms)
{
    if (m_session == SessionType::Default)
        return;

    if (m_s3_deadline > 0 && tick_ms > m_s3_deadline) {
        m_timed_out = true;
        m_session   = SessionType::Default;
        m_s3_deadline = 0;
    }
}

void UdsSession::keep_alive(uint32_t tick_ms)
{
    m_timed_out   = false;
    m_s3_deadline = tick_ms + m_timing.s3_ms;
}

} // namespace uds
