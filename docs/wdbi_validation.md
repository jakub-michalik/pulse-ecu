# WriteDataByIdentifier (0x2E) – Validation Rules

## Request format
```
[0x2E] [DID high] [DID low] [data bytes...]
```
Minimum length: 4 bytes (SID + 2-byte DID + at least 1 data byte)

## Validation sequence
1. Check length >= 4                     → NRC 0x13 if not
2. Extract DID (bytes 1-2, big-endian)
3. Look up DID in registered handlers    → NRC 0x31 if not found
4. Check session (usually Extended only) → NRC 0x7F if wrong session
5. Check security level if required      → NRC 0x33 if locked
6. Validate data length for this DID     → NRC 0x13 if wrong
7. Call write handler
8. Return positive response: [0x6E] [DID high] [DID low]

## Comparison: RDBI vs WDBI
| Aspect          | RDBI (0x22)         | WDBI (0x2E)              |
|-----------------|---------------------|--------------------------|
| Session         | Default + Extended  | Extended only (typically)|
| Security        | Usually not needed  | Often level 1 required   |
| Response        | DID + data          | DID echo only            |
| Typical size    | Up to 255 bytes     | DID-specific             |

## Common writable DIDs
- 0xF186 – Active Diagnostic Session (read-back)
- 0xF18C – ECU Serial Number
- 0xF190 – VIN
- 0xF197 – System supplier-specific identifier
