# P2 / P2* Server Timing – Implementation Details

## Why both P2 and P2*?
Short services (TesterPresent, ECUReset) respond within P2.
Long operations (flash erase, seed generation) need P2* with interim NRC 0x78.

## How uds_server handles it
1. Service `handle()` is called
2. If service returns `ResponsePending`, server sends NRC 0x78 and marks `m_pending = true`
3. `tick()` is called periodically; if pending response ready, sends it
4. Total time from request to final response must not exceed p2ext_ms

## Practical values for STM32 projects
- Flash page erase on STM32F4: ~20-40 ms per sector → use NRC 0x78 after first sector
- CRC calculation over 256 KB: ~5 ms at 168 MHz → fits within P2
- Seed generation (LFSR): < 1 ms → fits within P2

## Configuration example
```cpp
TimingConfig timing;
timing.p2_ms    = 50;     // 50 ms strict
timing.p2ext_ms = 5000;   // 5 s for flash ops
timing.s3_ms    = 5000;   // 5 s session keep-alive
server.set_timing(timing);
```
