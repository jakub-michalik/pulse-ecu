# Negative Response Code Handling

## When to send NRC vs. positive response
Services must check preconditions and return NRC if they fail:

```
1. Check message length                → NRC 0x13 if wrong
2. Check sub-function validity         → NRC 0x12 if unknown
3. Check session requirements          → NRC 0x7F if wrong session
4. Check security level                → NRC 0x33 if not unlocked
5. Execute service logic
6. Check resource availability         → NRC 0x22 if conditions not met
7. Return positive response
```

## Common NRC codes used in this library

| NRC  | Name                                      | Typical cause                   |
|------|-------------------------------------------|---------------------------------|
| 0x10 | generalReject                             | catch-all                       |
| 0x11 | serviceNotSupported                       | SID not registered              |
| 0x12 | subFunctionNotSupported                   | unknown sub-function byte       |
| 0x13 | incorrectMessageLength                    | wrong DLC or payload size       |
| 0x22 | conditionsNotCorrect                      | precondition check failed       |
| 0x24 | requestSequenceError                      | wrong order (e.g. TD before RD) |
| 0x31 | requestOutOfRange                         | DID/address out of bounds       |
| 0x33 | securityAccessDenied                      | security level not met          |
| 0x35 | invalidKey                                | wrong seed-key response         |
| 0x36 | exceededNumberOfAttempts                  | brute-force lockout             |
| 0x37 | requiredTimeDelayNotExpired               | delay timer still active        |
| 0x7F | serviceNotSupportedInActiveSession        | wrong session                   |
| 0x78 | requestCorrectlyReceivedResponsePending   | ECU needs more processing time  |

## write_nrc() helper in service_base.hpp
All service implementations use `write_nrc(buf, nrc)` which formats
the standard 3-byte NR PDU: `[0x7F, SID, NRC]`.
