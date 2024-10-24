#pragma once

#include <cstdint>
#include <cstddef>

namespace uds {
namespace dtc {

// DTC Status bit masks (ISO 14229-1 Table D.1)
struct DtcStatus {
    static constexpr uint8_t kTestFailed               = 0x01;
    static constexpr uint8_t kTestFailedThisOpCycle    = 0x02;
    static constexpr uint8_t kPendingDtc               = 0x04;
    static constexpr uint8_t kConfirmedDtc             = 0x08;
    static constexpr uint8_t kTestNotCompletedSinceLastClear = 0x10;
    static constexpr uint8_t kTestFailedSinceLastClear = 0x20;
    static constexpr uint8_t kTestNotCompletedThisOpCycle   = 0x40;
    static constexpr uint8_t kWarningIndicatorRequested     = 0x80;
};

// DTC Severity (ISO 14229-1)
struct DtcSeverity {
    static constexpr uint8_t kNoSeverity        = 0x00;
    static constexpr uint8_t kMaintenance       = 0x20;
    static constexpr uint8_t kCheckAtNextHalt   = 0x40;
    static constexpr uint8_t kCheckImmediately  = 0x80;
};

// DTC functional unit categories
enum class DtcFunctionalGroup : uint32_t {
    AllDtcs          = 0xFFFFFF,
    EmissionSystem   = 0xFEFFFF,
    SafetySystem     = 0xFDFFFF,
};

// Snapshot data record
struct SnapshotRecord {
    uint8_t  record_number;
    uint16_t dids[8];
    uint8_t  did_count;
};

// A single DTC entry
struct DtcEntry {
    uint32_t dtc_code;   // 3-byte DTC (MSB=0)
    uint8_t  status;
    uint8_t  severity;
    bool     active;

    // Snapshot data (freeze frame)
    uint8_t  snapshot[64];
    size_t   snapshot_len;

    // Extended data
    uint8_t  ext_data[32];
    size_t   ext_data_len;
};

} // namespace dtc
} // namespace uds

// Extended data record numbers (ISO 14229-1 Table D.3)
namespace extended_data_record {
    static constexpr uint8_t kFaultOccurrenceCounter  = 0x01;
    static constexpr uint8_t kAgingCounter            = 0x02;
    static constexpr uint8_t kPrefailureData          = 0x10;
    static constexpr uint8_t kAllRecords              = 0xFF;
    static constexpr uint8_t kOBDRecords              = 0xFE;
}
