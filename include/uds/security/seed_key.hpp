#pragma once

#include <cstdint>
#include <cstddef>

namespace uds {
namespace security {

/**
 * Seed/Key algorithm interface.
 *
 * Standard approach: ECU sends a random seed, tester transforms it using
 * a shared secret algorithm to produce the key. ECU verifies the key.
 *
 * This header provides common algorithm implementations and interface.
 */

// XOR-based simple algorithm (not production-safe, for development/testing only)
struct XorAlgorithm {
    uint32_t secret_mask;  // XOR mask applied to seed to produce key

    // Generate a 4-byte seed (use hardware RNG or LFSR in production)
    static size_t generate_seed(uint8_t* buf, size_t max, void* ctx);

    // Verify key by XOR-ing seed with mask
    static bool verify_key(const uint8_t* seed, size_t seed_len,
                           const uint8_t* key, size_t key_len,
                           void* ctx);
};

// LFSR-based seed generator
class LfsrGenerator {
public:
    explicit LfsrGenerator(uint32_t seed = 0xACE1D00D);

    uint32_t next();
    void fill(uint8_t* buf, size_t len);

private:
    uint32_t m_state;
};

} // namespace security
} // namespace uds
