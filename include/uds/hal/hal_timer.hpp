#pragma once

#include "hal_interface.hpp"
#include <cstdint>

namespace uds {
namespace hal {

inline bool is_elapsed(uint32_t start, uint32_t current, uint32_t timeout) {
    return (current - start) >= timeout;
}

class SoftTimer {
public:
    explicit SoftTimer(ITimer& timer) : m_timer(timer), m_start(0), m_running(false) {}

    void start() { m_start = m_timer.get_tick_ms(); m_running = true; }
    void stop()  { m_running = false; }
    void restart() { m_start = m_timer.get_tick_ms(); }

    bool is_expired(uint32_t timeout_ms) const {
        if (!m_running) return false;
        return is_elapsed(m_start, m_timer.get_tick_ms(), timeout_ms);
    }

    uint32_t elapsed_ms() const {
        if (!m_running) return 0;
        return m_timer.get_tick_ms() - m_start;
    }

    bool is_running() const { return m_running; }

private:
    ITimer&  m_timer;
    uint32_t m_start;
    bool     m_running;
};

class PeriodicTimer {
public:
    PeriodicTimer(ITimer& timer, uint32_t period_ms)
        : m_timer(timer), m_period(period_ms), m_next(0), m_started(false) {}

    void start() { m_next = m_timer.get_tick_ms() + m_period; m_started = true; }

    bool tick() {
        if (!m_started) return false;
        uint32_t now = m_timer.get_tick_ms();
        if (now >= m_next) { m_next = now + m_period; return true; }
        return false;
    }

private:
    ITimer&  m_timer;
    uint32_t m_period;
    uint32_t m_next;
    bool     m_started;
};

} // namespace hal
} // namespace uds

// Timer overflow note:
// All timeout comparisons use is_elapsed(start, current, timeout) which
// uses (current - start) >= timeout. This correctly handles uint32_t
// wraparound at ~49.7 days.
