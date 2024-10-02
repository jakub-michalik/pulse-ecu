# ISO-TP STmin Timing – Clarification

## STmin field encoding (ISO 15765-2 Table 4)
| Value range | Unit         | Min separation time    |
|-------------|--------------|------------------------|
| 0x00        | –            | 0 ms (send immediately)|
| 0x01–0x7F   | milliseconds | 1–127 ms               |
| 0x80–0xF0   | reserved     | treat as 127 ms        |
| 0xF1–0xF9   | 100 µs steps | 100–900 µs             |
| 0xFA–0xFF   | reserved     | treat as 127 ms        |

## Current implementation
The current `handle_fc()` extracts STmin but does not yet implement
the sub-millisecond 0xF1-0xF9 range. All STmin values are treated
as milliseconds because `get_tick_ms()` only has 1 ms resolution.

For STM32 targets with HAL tick, this is acceptable – most production
testers use STmin=0x00 or 0x0A (10 ms).

## TODO
If sub-ms precision is needed, use `DWT->CYCCNT` cycle counter on
Cortex-M3/M4/M7 to achieve ~6 ns resolution at 168 MHz.

## Tested STmin values
- 0x00 (0 ms)   – works on STM32F4 at up to 500 kbps
- 0x0A (10 ms)  – standard for most AUTOSAR stacks
- 0x14 (20 ms)  – conservative for slow targets
