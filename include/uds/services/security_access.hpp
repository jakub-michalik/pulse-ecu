#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

static constexpr size_t kMaxSecurityLevels = 8;
static constexpr uint8_t kMaxAttempts      = 3;
static constexpr uint32_t kDelayAfterFail  = 10000; // 10 seconds

struct SecurityLevel {
    uint8_t level;          // Odd number: requestSeed subfunction
    uint8_t seed_len;       // Seed length in bytes

    // Generate seed - fill seed_buf with random seed, return length
    size_t (*generate_seed)(uint8_t* seed_buf, size_t max_len, void* ctx);

    // Verify key - return true if key is valid for given seed
    bool (*verify_key)(const uint8_t* seed, size_t seed_len,
                       const uint8_t* key,  size_t key_len,
                       void* ctx);

    // Required session for this level
    SessionType required_session;
    void*       ctx;
};

class SecurityAccessService : public IService {
public:
    SecurityAccessService();

    ServiceId service_id() const override { return ServiceId::SecurityAccess; }

    bool register_level(const SecurityLevel& level);

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

    // Call periodically with current tick - handles delay timer
    void update(uint32_t tick_ms);

    bool is_unlocked(uint8_t level) const;

private:
    struct LevelState {
        SecurityLevel cfg;
        uint8_t  seed[16];
        size_t   seed_len;
        bool     seed_sent;
        bool     unlocked;
        uint8_t  fail_count;
        uint32_t locked_until;
    };

    LevelState*       find_level(uint8_t level);
    const LevelState* find_level(uint8_t level) const;

    LevelState m_levels[kMaxSecurityLevels];
    size_t     m_level_count;
    uint8_t    m_active_level;
};

} // namespace uds
