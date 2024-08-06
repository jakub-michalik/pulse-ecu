# ISO-TP Implementation Notes (ISO 15765-2)

## Frame types
| PCI nibble | Type | Max data |
|------------|------|----------|
| 0x0        | SF   | 7 bytes  |
| 0x1        | FF   | first 6 bytes of multi-frame |
| 0x2        | CF   | 7 bytes continuation |
| 0x3        | FC   | flow control (BS, STmin) |

## Sequence number
- First CF carries SN=1, wraps at 0xF back to 0x0
- Out-of-order CF → abort reception, send error

## Flow Control fields
- `FS=0` Continue, `FS=1` Wait, `FS=2` Overflow
- `BS`  – block size (0 = send all CFs without pause)
- `STmin` – 0x00-0x7F ms, 0xF1-0xF9 = 100-900 µs

## Timeouts
- N_Bs: time to receive FC after FF (default 1000 ms)
- N_Cr: time between consecutive CFs (default 150 ms)
