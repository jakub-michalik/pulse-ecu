# DTC Status Byte Reference (ISO 14229-1 §D.2)

## Status byte bit definitions

| Bit | Symbol                          | Meaning                                    |
|-----|---------------------------------|--------------------------------------------|
| 0   | testFailed                      | Most recent test result: failed            |
| 1   | testFailedThisOperationCycle    | Failed at least once this cycle            |
| 2   | pendingDTC                      | Failed in current or previous cycle        |
| 3   | confirmedDTC                    | Confirmed after threshold exceeded         |
| 4   | testNotCompletedSinceLastClear  | Test not run since last DTC clear          |
| 5   | testFailedSinceLastClear        | Failed at least once since clear           |
| 6   | testNotCompletedThisCycle       | Test has not run in current cycle          |
| 7   | warningIndicatorRequested       | Warning lamp should be illuminated         |

## DtcStatus flags in dtc.hpp
```cpp
enum class DtcStatus : uint8_t {
    TestFailed                     = 0x01,
    TestFailedThisOpCycle          = 0x02,
    PendingDTC                     = 0x04,
    ConfirmedDTC                   = 0x08,
    TestNotCompletedSinceLastClear = 0x10,
    TestFailedSinceLastClear       = 0x20,
    TestNotCompletedThisCycle      = 0x40,
    WarningIndicatorRequested      = 0x80,
};
```

## Availability mask
Not all ECUs implement all bits. The availability mask indicates
which bits are supported. Default: 0xFF (all supported).
