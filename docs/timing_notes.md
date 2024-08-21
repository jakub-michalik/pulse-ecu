# UDS Timing Configuration

## Timing parameters (ISO 14229-1 Table 5)

| Parameter      | Symbol       | Default  | Description                              |
|----------------|--------------|----------|------------------------------------------|
| p2ServerMax    | P2_SERVER    | 50 ms    | Max response time for simple requests    |
| p2*ServerMax   | P2EXT_SERVER | 5000 ms  | Max response time with NRC 0x78          |
| s3Server       | S3_SERVER    | 5000 ms  | Session keep-alive timeout               |

## NRC 0x78 – Response Pending
When the ECU needs more time than p2ServerMax, it must:
1. Send NRC 0x78 (requestCorrectlyReceivedResponsePending) within p2ServerMax
2. Send the actual response within p2*ServerMax
3. May repeat NRC 0x78 up to the p2*ServerMax deadline

## TimingConfig struct fields
```cpp
struct TimingConfig {
    uint32_t p2_ms;      // P2 server max (default 50)
    uint32_t p2ext_ms;   // P2* server max (default 5000)
    uint32_t s3_ms;      // S3 server timeout (default 5000)
};
```
