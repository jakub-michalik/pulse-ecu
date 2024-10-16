# Session Timing Matrix

## Allowed services per session (ISO 14229-1 Annex B)

| Service (SID)              | Default | Extended | Programming |
|----------------------------|---------|----------|-------------|
| TesterPresent (0x3E)       | ✓       | ✓        | ✓           |
| DiagnosticSessionCtrl(0x10)| ✓       | ✓        | ✓           |
| ECUReset (0x11)            | ✓       | ✓        | ✓           |
| ReadDTCInformation (0x19)  | ✓       | ✓        | –           |
| ReadDataByIdentifier(0x22) | ✓       | ✓        | –           |
| SecurityAccess (0x27)      | –       | ✓        | ✓           |
| WriteDataByIdentifier(0x2E)| –       | ✓        | –           |
| RoutineControl (0x31)      | –       | ✓        | ✓           |
| RequestDownload (0x34)     | –       | –        | ✓           |
| TransferData (0x36)        | –       | –        | ✓           |
| RequestTransferExit (0x37) | –       | –        | ✓           |
| CommunicationControl(0x28) | –       | ✓        | ✓           |
| ControlDTCSetting (0x85)   | –       | ✓        | ✓           |

## Timing values used in pulse-ecu

| Session     | p2_ms | p2ext_ms | s3_ms |
|-------------|-------|----------|-------|
| Default     | 50    | 5000     | N/A   |
| Extended    | 50    | 5000     | 5000  |
| Programming | 50    | 5000     | 5000  |

Note: p2ext is used when ECU sends NRC 0x78 (ResponsePending).
