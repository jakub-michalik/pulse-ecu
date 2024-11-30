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

    // Get the supported status bits for this ECU
    uint8_t status_availability_mask() const { return m_status_availability; }

    // Set which status bits are supported
    void set_status_availability(uint8_t mask) { m_status_availability = mask; }

private:
    uint8_t m_status_availability = 0xFF;

    bool set_freeze_frame(uint32_t code, uint8_t record_num,
                          const uint8_t* data, size_t len);
    size_t get_freeze_frame(uint32_t code, uint8_t record_num,
                            uint8_t* out, size_t max_len) const;

    bool is_confirmed(uint32_t code) const {
        for (size_t i = 0; i < m_count; ++i)
            if (m_dtcs[i].dtc_code == (code & 0xFFFFFF))
                return (m_dtcs[i].status & DtcStatus::kConfirmedDtc) != 0;
        return false;
    }
