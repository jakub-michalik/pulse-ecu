# Flash Programming Sequence (ISO 14229-1 §14)

## Standard download flow
```
1. DiagnosticSessionControl(0x02)     – enter Programming session
2. SecurityAccess(0x27, 0x01)         – request seed
3. SecurityAccess(0x27, 0x02, key)    – send key, unlock level 1
4. CommunicationControl(0x03, 0x01)   – disable Rx+Tx on network
5. RoutineControl(0x31, 0x01, 0xFF00) – erase memory (flash routine)
   ← NRC 0x78 while erasing (can take 200-500 ms per sector)
6. RequestDownload(addr, size, method) – open download session
7. TransferData(block1)               – first 256-byte block
8. TransferData(block2)               – second block
   ... repeat until all data sent
9. RequestTransferExit()              – close session
10. RoutineControl(0x31, 0x01, 0xFF01) – checksum verify routine
11. ECUReset(0x01)                    – reset to run new firmware
```

## Block counter wrapping
TransferData uses a 1-byte block sequence counter (0x01–0xFF, then wraps to 0x00).
Formula: `expected_BSC = (last_BSC + 1) & 0xFF`

## RequestDownload compression/encryption byte
Byte 3 of RequestDownload: `[encMethod(4 bits)] [compMethod(4 bits)]`
- 0x00 = no compression, no encryption (most common)
- 0x11 = compressed, encrypted (OEM-specific)

## Practical block size for STM32F4
Flash page = 16 KB (sectors 0-3) / 64 KB (sector 4) / 128 KB (sectors 5-11)
Recommended TransferData block: 512–4096 bytes (balance between speed and NRC 0x78 frequency)
