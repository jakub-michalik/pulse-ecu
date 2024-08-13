# UDS Session State Machine

## Session types (ISO 14229-1 §7.4)
- `0x01` Default            – always available, limited services
- `0x02` Programming        – flash download, restricted
- `0x03` Extended           – full diagnostics, security required

## State transitions
```
Default ──── DiagnosticSessionControl(0x02) ──▶ Programming
Default ──── DiagnosticSessionControl(0x03) ──▶ Extended
Programming ─ DiagnosticSessionControl(0x01) ─▶ Default
Extended ──── DiagnosticSessionControl(0x01) ─▶ Default
Any ────────── EcuReset ──────────────────────▶ Default
Any ────────── S3 timeout ────────────────────▶ Default
```

## S3 server timer
- Starts when a non-default session is entered
- Reset by any valid request (incl. TesterPresent)
- Default value: 5000 ms (p2ServerMax = 50 ms, p2*ServerMax = 5000 ms)
- On expiry: automatic transition back to Default session

## Implementation notes
- `keep_alive()` resets the S3 deadline in uds_session.cpp
- `process()` checks `m_timed_out` flag set by `tick()`
