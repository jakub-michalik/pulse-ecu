# InputOutputControlByIdentifier (0x2F) – Control Modes

## Sub-function: inputOutputControlParameter (byte 3)
| Value | Name                  | Meaning                                   |
|-------|-----------------------|-------------------------------------------|
| 0x00  | returnControlToECU    | ECU resumes normal control                |
| 0x01  | resetToDefault        | Reset to default/factory value            |
| 0x02  | freezeCurrentState    | Hold current output value                 |
| 0x03  | shortTermAdjustment   | Apply value from request                  |

## Request format
```
[0x2F] [DID high] [DID low] [controlParam] [controlState (optional)]
```
`controlState` only present when controlParam = 0x03 (shortTermAdjustment).

## Typical use cases
- Override cooling fan PWM during EOL calibration
- Force fuel injector open for leak test
- Activate indicator lamp for visual verification
- Set sensor simulation value during bench test

## Safety considerations
- IO control only allowed in Extended session
- ECU MUST restore normal control on:
  - Session timeout (S3 expiry)
  - `returnControlToECU` sub-function call
  - ECU reset
- Active IO overrides should be tracked in `m_active_controls[]`
  so they can all be restored at once on session exit

## Implementation note
Current impl supports single active control at a time.
For multi-output control, extend `IService` with a `on_session_exit()` hook.
