# Changelog

All notable changes to pulse-ecu are documented here.

## [1.0.0] - 2024-12-23

### Added
- Full ISO 14229-1 UDS service set (20 services)
- ISO 15765-2 (ISO-TP) transport layer
- Session management (default/programming/extended) with S3 timeout
- SecurityAccess service with configurable seed/key algorithm
- DTC manager with freeze frame and extended data record support
- Flash programming routines (erase/verify)
- CRC-32 and CRC-16 CCITT utilities
- STM32F1/F4/H7 CAN/FDCAN HAL drivers
- HAL abstraction layer (ITimer, IRng, INvs)
- CMake build system with ARM cross-compilation toolchain
- Unit tests for ISO-TP, session management, and timing
- STM32F4 full example with all major services

### Services implemented
- 0x10 DiagnosticSessionControl
- 0x11 ECUReset
- 0x14 ClearDiagnosticInformation
- 0x19 ReadDTCInformation
- 0x22 ReadDataByIdentifier
- 0x23 ReadMemoryByAddress
- 0x27 SecurityAccess
- 0x28 CommunicationControl
- 0x2E WriteDataByIdentifier
- 0x2F InputOutputControlByIdentifier
- 0x31 RoutineControl
- 0x34 RequestDownload
- 0x35 RequestUpload
- 0x36 TransferData
- 0x37 RequestTransferExit
- 0x3D WriteMemoryByAddress
- 0x3E TesterPresent
- 0x85 ControlDTCSetting
- 0x86 ResponseOnEvent
- 0x87 LinkControl

### Notes
- C++14 standard, no dynamic allocation, no exceptions
- Tested on STM32F407 @ 500kbps CAN

## [0.9.0] - 2024-11-05

Initial pre-release with core functionality.

## [0.1.0] - 2024-07-29

Initial development started.
