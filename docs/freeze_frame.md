# DTC Freeze Frame – Design Notes

## What is a freeze frame?
A snapshot of sensor/system data captured at the moment a DTC is set.
Used to recreate the conditions that triggered the fault.

## ISO 14229-1 freeze frame structure
Each freeze frame record contains:
- Record number (0x01–0xFE; 0xFF = all records)
- One or more DIDs with their values at time of fault

## Implementation in dtc_manager
```cpp
struct FreezeFrameRecord {
    uint8_t  record_number;
    uint16_t did;
    uint8_t  data[MAX_FREEZE_FRAME_DATA];
    uint8_t  data_len;
};
```
Stored per DtcEntry: up to `MAX_FREEZE_FRAMES` records per DTC.

## ReadDTCInformation sub-function 0x04
`reportDTCSnapshotRecordByDTCNumber` – returns freeze frame for a specific DTC:
```
Request:  [0x19] [0x04] [DTC high] [DTC mid] [DTC low] [record#]
Response: [0x59] [0x04] [DTC bytes] [statusMask] [record#] [DID] [data...]
```

## When to capture freeze frame
Capture on first failed test (when `TestFailed` bit transitions 0→1).
Do NOT overwrite existing freeze frame unless explicitly cleared with
`ClearDiagnosticInformation (0x14)`.

## Memory usage estimate
With MAX_FREEZE_FRAMES=3, MAX_FREEZE_FRAME_DATA=16, MAX_DTC=32:
  32 × 3 × (1+2+16+1) = 32 × 3 × 20 = 1920 bytes (~2 KB)
Acceptable for STM32F4 with 192 KB RAM.
