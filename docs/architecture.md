# Pulse ECU – Architecture Overview

## Layer structure

```
Application
    │
UDS Services (ISO 14229-1)
    │
UDS Server / Session Manager
    │
ISO-TP Transport (ISO 15765-2)
    │
CAN Interface abstraction
    │
STM32 HAL driver
```

## Key design decisions
- No dynamic allocation – all buffers are compile-time sized (`StaticBuffer<N>`)
- No exceptions, no RTTI – suitable for bare-metal targets
- Service registration at startup via `register_service()`
- C++14 selected for compatibility with arm-none-eabi-gcc 10+

## Directory layout
- `include/uds/` – public headers
- `src/uds/`     – service and transport implementations
- `stm32/`       – STM32-specific CAN drivers
- `examples/`    – sample integrations
