# pulse-ecu

UDS (Unified Diagnostic Services) library for STM32 microcontrollers.

Implements ISO 14229-1 UDS protocol stack over ISO 15765-2 (ISO-TP) transport on CAN bus.
Designed for resource-constrained embedded targets: no dynamic allocation, no exceptions, C++14.

## Supported Services

| SID  | Service                        | Impl |
|------|--------------------------------|------|
| 0x10 | DiagnosticSessionControl       | ✓    |
| 0x11 | ECUReset                       | ✓    |
| 0x14 | ClearDiagnosticInformation     | ✓    |
| 0x19 | ReadDTCInformation             | ✓    |
| 0x22 | ReadDataByIdentifier           | ✓    |
| 0x23 | ReadMemoryByAddress            | ✓    |
| 0x27 | SecurityAccess                 | ✓    |
| 0x28 | CommunicationControl           | ✓    |
| 0x2E | WriteDataByIdentifier          | ✓    |
| 0x2F | InputOutputControlByIdentifier | ✓    |
| 0x31 | RoutineControl                 | ✓    |
| 0x34 | RequestDownload                | ✓    |
| 0x35 | RequestUpload                  | ✓    |
| 0x36 | TransferData                   | ✓    |
| 0x37 | RequestTransferExit            | ✓    |
| 0x3D | WriteMemoryByAddress           | ✓    |
| 0x3E | TesterPresent                  | ✓    |
| 0x85 | ControlDTCSetting              | ✓    |
| 0x86 | ResponseOnEvent                | ✓    |
| 0x87 | LinkControl                    | ✓    |

## Hardware Support

- STM32F1xx (bxCAN)
- STM32F4xx (bxCAN)
- STM32H7xx (FDCAN) - in progress

## Requirements

- arm-none-eabi-gcc >= 10.0 (tested with 12.x)
- CMake >= 3.16
- C++14 standard

## Quick Start

```cpp
#include "uds/uds_server.hpp"
#include "uds/services/tester_present.hpp"
#include "uds/services/diagnostic_session_control.hpp"

// 1. Implement ICanInterface for your hardware
// 2. Create ISO-TP and UDS server
uds::transport::IsoTp    isotp(your_can_driver, cfg);
uds::UdsServer            server(isotp);

// 3. Register services
uds::TesterPresentService          tp;
uds::DiagnosticSessionControlService dsc;
server.register_service(&tp);
server.register_service(&dsc);

// 4. Call in main loop
while (true) {
    // Feed CAN frames to ISO-TP
    uds::transport::CanFrame f;
    while (your_can_driver.receive(f))
        isotp.process_frame(f);

    server.process();
}
```

## Build

```bash
# Host build (for testing)
cmake -B build -DPULSE_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build

# STM32F4 cross-build
cmake -B build-arm \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
  -DPULSE_BUILD_EXAMPLES=ON
cmake --build build-arm
```

## License

MIT
