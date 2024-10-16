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
    // Can always go to default
    if (new_session == SessionType::Default) {
        reset();
        return true;
    }

    // Programming session can only be entered from default
    if (new_session == SessionType::Programming &&
        m_session != SessionType::Default) {
        return false;
    }

    m_session   = new_session;
    m_timed_out = false;
    // S3 timer will be refreshed by keep_alive() called from server
    return true;
}

void UdsSession::reset()
{
    m_session     = SessionType::Default;
    m_timed_out   = false;
    m_s3_deadline = 0;
}

void UdsSession::update(uint32_t tick_ms)
{
    if (m_session == SessionType::Default) {
        m_s3_deadline = 0;
        m_timed_out   = false;
        return;
    }

    if (m_s3_deadline != 0 && tick_ms >= m_s3_deadline) {
        m_timed_out   = true;
        m_session     = SessionType::Default;
        m_s3_deadline = 0;
    }
}

void UdsSession::keep_alive(uint32_t tick_ms)
{
    m_timed_out   = false;
    if (m_session != SessionType::Default)
        m_s3_deadline = tick_ms + m_timing.s3_ms;
}

} // namespace uds

// Note: In default session, s3 timer is not started.
// The S3 timer only runs when in non-default sessions.
