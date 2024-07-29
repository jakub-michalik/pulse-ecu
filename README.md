# pulse-ecu

UDS (Unified Diagnostic Services) library for STM32 microcontrollers.

Implements ISO 14229-1 UDS protocol over ISO 15765-2 (ISO-TP) transport layer on CAN bus.

## Features

- Full ISO-TP (ISO 15765-2) transport layer implementation
- All major UDS services (ISO 14229-1)
- STM32 HAL CAN driver integration
- C++14, no exceptions, no dynamic allocation
- Designed for resource-constrained embedded targets

## Supported Services

| SID  | Service                        |
|------|--------------------------------|
| 0x10 | DiagnosticSessionControl       |
| 0x11 | ECUReset                       |
| 0x14 | ClearDiagnosticInformation     |
| 0x19 | ReadDTCInformation             |
| 0x22 | ReadDataByIdentifier           |
| 0x23 | ReadMemoryByAddress            |
| 0x27 | SecurityAccess                 |
| 0x28 | CommunicationControl           |
| 0x2E | WriteDataByIdentifier          |
| 0x2F | InputOutputControlByIdentifier |
| 0x31 | RoutineControl                 |
| 0x34 | RequestDownload                |
| 0x35 | RequestUpload                  |
| 0x36 | TransferData                   |
| 0x37 | RequestTransferExit            |
| 0x3D | WriteMemoryByAddress           |
| 0x3E | TesterPresent                  |
| 0x85 | ControlDTCSetting              |
| 0x86 | ResponseOnEvent                |
| 0x87 | LinkControl                    |

## Requirements

- arm-none-eabi-gcc >= 10.0
- CMake >= 3.16
- C++14 standard

## License

MIT
