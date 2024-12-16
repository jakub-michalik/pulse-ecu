# pulse-ecu

UDS (Unified Diagnostic Services) library for STM32 microcontrollers.

Implements ISO 14229-1 over ISO 15765-2 (ISO-TP) on CAN bus.
No dynamic memory allocation, no exceptions. C++14. MIT license.

## Features

- Complete UDS service set (20 services)
- ISO-TP transport layer with multi-frame support
- Session management with S3 timeout
- Security access with configurable seed/key algorithm
- DTC manager with freeze frame support
- STM32F1/F4/H7 CAN/FDCAN drivers
- CMake-based build system with ARM toolchain support

## Services

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

## Quick Start

```cpp
#include "uds/uds_server.hpp"
#include "uds/services/tester_present.hpp"
#include "uds/services/diagnostic_session_control.hpp"
// ... other services

// Implement ICanInterface for your hardware
// or use uds::stm32::Stm32Can / Stm32F1Can / Stm32H7Can

uds::transport::IsoTpConfig cfg = uds::transport::kDefaultIsoTpConfig;
cfg.rx_id = 0x7E0;  // tester request ID
cfg.tx_id = 0x7E8;  // ECU response ID

uds::transport::IsoTp isotp(your_can, cfg);
uds::UdsServer          server(isotp);

uds::TesterPresentService             tp;
uds::DiagnosticSessionControlService  dsc;
server.register_service(&tp);
server.register_service(&dsc);

// In main loop:
while (true) {
    uds::transport::CanFrame f;
    while (your_can.receive(f))
        isotp.process_frame(f);
    server.process();
}
```

## Build

```bash
# Host (Linux/macOS/Windows)
cmake -B build -DPULSE_BUILD_TESTS=ON -DPULSE_BUILD_EXAMPLES=ON
cmake --build build && ctest --test-dir build

# STM32F4 cross-compile
cmake -B build-arm \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
  -DPULSE_BUILD_STM32=ON
cmake --build build-arm
```

## Hardware Support

| MCU Family | Peripheral | Driver              |
|------------|------------|---------------------|
| STM32F4xx  | bxCAN      | stm32/stm32_can.hpp  |
| STM32F1xx  | bxCAN      | stm32/stm32f1_can.hpp|
| STM32H7xx  | FDCAN      | stm32/stm32h7_can.hpp|

## Version

1.0.0

## License

MIT - see LICENSE file.
