#pragma once

#include "dtc.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {
namespace dtc {

static constexpr size_t kMaxDtcs = 64;

class DtcManager {
public:
    DtcManager();

    // Register a DTC - returns index or -1 on error
    int register_dtc(uint32_t code, uint8_t severity = DtcSeverity::kNoSeverity);

    // Set test result for a DTC
    void set_test_result(uint32_t code, bool failed);

    // Clear all DTCs (0x14 service)
    void clear_all();

    // Clear specific DTC group
    void clear_group(uint32_t group_mask);

    // Get number of DTCs matching status mask
    size_t count_by_status(uint8_t status_mask) const;

    // Iterate DTCs matching status mask
    // callback: return false to stop iteration
    void for_each(uint8_t status_mask,
                  bool (*callback)(const DtcEntry&, void* ctx),
                  void* ctx) const;

    // Find DTC by code
    DtcEntry* find(uint32_t code);

    // Update DTC status bits at end of operation cycle
    void update_op_cycle();

    // Store snapshot data for a DTC
    bool set_snapshot(uint32_t code, const uint8_t* data, size_t len);

    // Get all DTCs array (for direct access)
    const DtcEntry* entries() const { return m_dtcs; }
    size_t          count()   const { return m_count; }

private:
    DtcEntry m_dtcs[kMaxDtcs];
    size_t   m_count;
};

} // namespace dtc
} // namespace uds
