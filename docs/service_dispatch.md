# Service Dispatch Architecture

## Registration
Services are registered at startup using `UdsServer::register_service()`.
The server holds a fixed-size array of `IService*` pointers (no heap).

```cpp
server.register_service(&tester_present_svc);
server.register_service(&diagnostic_session_svc);
server.register_service(&security_access_svc);
// ... up to MAX_SERVICES (default 32)
```

## Dispatch flow
```
receive CAN frame
    │
IsoTp::process_frame()   ← reassemble multi-frame if needed
    │
UdsServer::process()
    │
find_service(sid)        ← linear search O(n), n≤32 → negligible
    │
session check            ← is service allowed in current session?
    │
security check           ← is required security level unlocked?
    │
IService::handle()       ← service-specific processing
    │
send response via IsoTp
```

## Why linear search?
With at most 20 standard UDS services registered, a linear scan of
a 32-element pointer array is faster than a hash map on Cortex-M4
because the array fits in L1 cache (~128 bytes).

## Refactoring notes (this commit)
- Extracted `find_service()` as a private method
- Added early return when service not found (avoids null-ptr check later)
- Kept `m_services[]` as raw pointer array for zero overhead
