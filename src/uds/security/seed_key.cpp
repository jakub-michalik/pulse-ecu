#include "uds/security/seed_key.hpp"
#include <cstring>

namespace uds {
namespace security {

size_t XorAlgorithm::generate_seed(uint8_t* buf, size_t max, void* ctx)
{
    (void)ctx;
    if (max < 4) return 0;
    static uint32_t lfsr = 0xDEADBEEF;
    lfsr ^= (lfsr << 13);
    lfsr ^= (lfsr >> 17);
    lfsr ^= (lfsr << 5);
    buf[0] = static_cast<uint8_t>(lfsr >> 24);
    buf[1] = static_cast<uint8_t>(lfsr >> 16);
    buf[2] = static_cast<uint8_t>(lfsr >> 8);
    buf[3] = static_cast<uint8_t>(lfsr);
    return 4;
}

bool XorAlgorithm::verify_key(
    const uint8_t* seed, size_t seed_len,
    const uint8_t* key,  size_t key_len,
    void* ctx)
{
    XorAlgorithm* self = reinterpret_cast<XorAlgorithm*>(ctx);
    if (!self || seed_len != 4 || key_len != 4) return false;
    uint32_t s = (static_cast<uint32_t>(seed[0]) << 24) |
                 (static_cast<uint32_t>(seed[1]) << 16) |
                 (static_cast<uint32_t>(seed[2]) << 8)  |
                  static_cast<uint32_t>(seed[3]);
    uint32_t k = (static_cast<uint32_t>(key[0]) << 24) |
                 (static_cast<uint32_t>(key[1]) << 16) |
                 (static_cast<uint32_t>(key[2]) << 8)  |
                  static_cast<uint32_t>(key[3]);
    return (s ^ self->secret_mask) == k;
}

LfsrGenerator::LfsrGenerator(uint32_t seed) : m_state(seed ? seed : 1) {}

uint32_t LfsrGenerator::next()
{
    uint32_t lsb = m_state & 1;
    m_state >>= 1;
    if (lsb) m_state ^= 0xB4BCD35CU;
    return m_state;
}

void LfsrGenerator::fill(uint8_t* buf, size_t len)
{
    size_t i = 0;
    while (i < len) {
        uint32_t word = next();
        buf[i++] = static_cast<uint8_t>(word >> 24);
        if (i < len) buf[i++] = static_cast<uint8_t>(word >> 16);
        if (i < len) buf[i++] = static_cast<uint8_t>(word >> 8);
        if (i < len) buf[i++] = static_cast<uint8_t>(word);
    }
}

} // namespace security
} // namespace uds
