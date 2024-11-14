#include "uds/services/security_access.hpp"
#include "uds/uds_session.hpp"
#include <cstring>

namespace uds {

SecurityAccessService::SecurityAccessService()
    : m_level_count(0)
    , m_active_level(0)
    , m_current_tick(0)
{
    memset(m_levels, 0, sizeof(m_levels));
}

bool SecurityAccessService::register_level(const SecurityLevel& level)
{
    if (m_level_count >= kMaxSecurityLevels) return false;
    if ((level.level & 1) == 0) return false;

    m_levels[m_level_count].cfg       = level;
    m_levels[m_level_count].seed_len  = 0;
    m_levels[m_level_count].seed_sent = false;
    m_levels[m_level_count].unlocked  = false;
    m_levels[m_level_count].fail_count   = 0;
    m_levels[m_level_count].locked_until = 0;
    m_level_count++;
    return true;
}

SecurityAccessService::LevelState* SecurityAccessService::find_level(uint8_t level)
{
    uint8_t seed_sub = (level & 1) ? level : static_cast<uint8_t>(level - 1);
    for (size_t i = 0; i < m_level_count; ++i)
        if (m_levels[i].cfg.level == seed_sub) return &m_levels[i];
    return nullptr;
}

bool SecurityAccessService::is_unlocked(uint8_t level) const
{
    for (size_t i = 0; i < m_level_count; ++i)
        if (m_levels[i].cfg.level == level) return m_levels[i].unlocked;
    return false;
}

void SecurityAccessService::update(uint32_t tick_ms)
{
    m_current_tick = tick_ms;
    for (size_t i = 0; i < m_level_count; ++i) {
        if (m_levels[i].locked_until > 0 && tick_ms >= m_levels[i].locked_until) {
            m_levels[i].locked_until = 0;
            m_levels[i].fail_count   = 0;
        }
    }
}

bool SecurityAccessService::handle(
    const uint8_t* req,
    size_t         req_len,
    uint8_t*       resp,
    size_t&        resp_len,
    UdsSession&    session)
{
    if (req_len < 2) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::IncorrectMessageLengthOrInvalidFormat);
        return true;
    }

    uint8_t sub_func = req[1];
    bool    is_seed  = (sub_func & 1) != 0;

    LevelState* lvl = find_level(sub_func);
    if (!lvl) {
        write_nrc(resp, resp_len, service_id(), NrcCode::SubFunctionNotSupported);
        return true;
    }

    if (lvl->locked_until != 0 && m_current_tick < lvl->locked_until) {
        write_nrc(resp, resp_len, service_id(), NrcCode::RequiredTimeDelayNotExpired);
        return true;
    }

    if (lvl->cfg.required_session != session.current()) {
        write_nrc(resp, resp_len, service_id(),
                  NrcCode::ServiceNotSupportedInActiveSession);
        return true;
    }

    if (is_seed) {
        if (lvl->unlocked) {
            resp[0] = static_cast<uint8_t>(ServiceId::SecurityAccess) + 0x40;
            resp[1] = sub_func;
            memset(resp + 2, 0, lvl->cfg.seed_len);
            resp_len = 2 + lvl->cfg.seed_len;
            return true;
        }

        lvl->seed_len  = lvl->cfg.generate_seed(lvl->seed, sizeof(lvl->seed), lvl->cfg.ctx);
        lvl->seed_sent = true;
        m_active_level = sub_func;

        resp[0] = static_cast<uint8_t>(ServiceId::SecurityAccess) + 0x40;
        resp[1] = sub_func;
        memcpy(resp + 2, lvl->seed, lvl->seed_len);
        resp_len = 2 + lvl->seed_len;
        return true;
    } else {
        if (!lvl->seed_sent || m_active_level != static_cast<uint8_t>(sub_func - 1)) {
            write_nrc(resp, resp_len, service_id(), NrcCode::RequestSequenceError);
            return true;
        }

        size_t key_len = req_len - 2;
        bool   valid   = lvl->cfg.verify_key(
            lvl->seed, lvl->seed_len, req + 2, key_len, lvl->cfg.ctx);

        if (!valid) {
            lvl->fail_count++;
            lvl->seed_sent = false;
            if (lvl->fail_count >= kMaxAttempts) {
                lvl->locked_until = m_current_tick + kDelayAfterFail;
                write_nrc(resp, resp_len, service_id(), NrcCode::ExceededNumberOfAttempts);
            } else {
                write_nrc(resp, resp_len, service_id(), NrcCode::InvalidKey);
            }
            return true;
        }

        lvl->unlocked    = true;
        lvl->fail_count  = 0;
        lvl->seed_sent   = false;

        resp[0]  = static_cast<uint8_t>(ServiceId::SecurityAccess) + 0x40;
        resp[1]  = sub_func;
        resp_len = 2;
        return true;
    }
}

} // namespace uds

// Note: fail_count reset on both successful unlock AND delay expiry (in update())

// Security access cleanup note:
// seed_sent is cleared on:
//   1. Failed key verification
//   2. Successful unlock
//   3. Session timeout (handled by UdsSession)
// level.unlocked is cleared when session returns to default
