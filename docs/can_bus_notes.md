# CAN Bus Notes for UDS Integration

## Classic CAN frame limits
- Max data field: 8 bytes
- ISO-TP uses all 8 bytes (PCI byte + up to 7 data bytes for SF)
- CAN ID: 11-bit or 29-bit extended; typical UDS uses 0x7E0/0x7E8 pair

## Standard tester/ECU CAN IDs
| Direction  | ID (physical) | Meaning              |
|------------|---------------|----------------------|
| Tester→ECU | 0x7E0         | functional addressing |
| ECU→Tester | 0x7E8         | ECU response          |
| Tester→all | 0x7DF         | functional broadcast  |

## Bit timing for common baud rates
| Baud rate | TQ at 42 MHz | Sample point |
|-----------|-------------|--------------|
| 125 kbps  | 24 TQ       | 75%          |
| 250 kbps  | 12 TQ       | 75%          |
| 500 kbps  | 6 TQ        | 75%          |

## STM32 bxCAN filter setup
For UDS, configure one filter to pass 0x7E8 (ECU response ID).
Use mask mode: ID=0x7E8, Mask=0x7FF to accept only that ID.
```c
CAN_FilterTypeDef f;
f.FilterIdHigh   = 0x7E8 << 5;
f.FilterMaskHigh = 0x7FF << 5;
f.FilterMode     = CAN_FILTERMODE_IDMASK;
f.FilterScale    = CAN_FILTERSCALE_16BIT;
f.FilterActivation = ENABLE;
HAL_CAN_ConfigFilter(&hcan1, &f);
```
