# CommunicationControl (0x28) – Design Notes

## Sub-function values (ISO 14229-1 §9.4)
| SF  | Name                              | Effect                          |
|-----|-----------------------------------|---------------------------------|
| 0x00 | enableRxAndTx                    | Normal operation                |
| 0x01 | enableRxAndDisableTx             | Listen-only mode                |
| 0x02 | disableRxAndEnableTx             | Transmit only                   |
| 0x03 | disableRxAndTx                   | Silence (flash programming use) |

## communicationType byte
- Bit 0-3: network segment (0x01 = normal comm, 0x02 = NM comm, 0x03 = both)
- Bit 4-7: reserved (shall be 0)

## Usage in flash programming sequence
Typical sequence before flash:
1. `CommunicationControl(0x03, 0x01)` – disable Rx+Tx on normal comms
2. Flash download sequence
3. `CommunicationControl(0x00, 0x01)` – re-enable after reset

## Implementation notes
- Service only allowed in non-default sessions
- ECU must track current comm state and restore on session timeout
- Current impl stores state in `m_comm_disabled` flag
