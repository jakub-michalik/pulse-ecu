# S3 Server Timer – Behavior Specification

## Purpose
The S3 server timer ensures the ECU returns to Default Session
if the tester stops communicating. This prevents the ECU from
staying in Programming or Extended session indefinitely.

## Trigger conditions
- Timer STARTS when ECU transitions to a non-default session
- Timer RESETS on any valid incoming request
- Timer EXPIRES → ECU transitions to Default session + NRC 0x78 pending
  responses are cancelled

## TesterPresent (0x3E)
Service exists specifically to keep the session alive.
- `suppressPosRspMsgIndicationBit` (bit 7 of subFunction) = 1 → no response
- Tester typically sends TesterPresent every 2000 ms when s3 = 5000 ms

## Implementation
```cpp
// In UdsSession::tick()
if (m_session != SessionType::Default && tick_ms >= m_s3_deadline) {
    m_timed_out = true;
    m_session = SessionType::Default;
}

// In UdsSession::keep_alive()
if (m_session != SessionType::Default)
    m_s3_deadline = tick_ms + m_timing.s3_ms;
```

## Edge case: session change resets timer
Switching from Programming→Extended (or vice versa) should restart
the timer. Current impl handles this in `change_session()`.
