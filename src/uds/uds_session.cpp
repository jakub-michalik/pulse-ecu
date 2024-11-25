#include "uds/uds_session.hpp"

namespace uds {

UdsSession::UdsSession(const TimingConfig& timing)
    : m_session(SessionType::Default)
    , m_timing(timing)
    , m_s3_deadline(0)
    , m_timed_out(false)
    , m_timeout_cb(nullptr)
    , m_timeout_ctx(nullptr)
{
}

bool UdsSession::transition(SessionType new_session)
{
    if (new_session == SessionType::Default) {
        reset();
        return true;
    }

    if (new_session == SessionType::Programming &&
        m_session != SessionType::Default) {
        return false;
    }

    m_session   = new_session;
    m_timed_out = false;
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

        if (m_timeout_cb) m_timeout_cb(m_timeout_ctx);
    }
}

void UdsSession::keep_alive(uint32_t tick_ms)
{
    m_timed_out = false;
    if (m_session != SessionType::Default)
        m_s3_deadline = tick_ms + m_timing.s3_ms;
}

} // namespace uds

// S3 server timer: starts on session transition, reset by keep_alive()
// Expiry returns ECU to default session (ISO 14229-1 Table 390)
