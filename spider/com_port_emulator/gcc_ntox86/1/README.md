# Slave 1 - escalator

- Device type: escalator (`metro_escalator.cpp`), device number (slave address) **1**
- Request: `01 04 00 00 00 26` - function 04, registers 30001-30038 (38 registers)
- Response file: address, 0x04, byte count `0x4c` (76 data bytes, 81-byte frame). No CRC - the emulator appends it.

## Files
| File | Scenario | Signals |
|---|---|---|
| `4.txt` / `4_baseline.txt` | Baseline: stopped, TU, ready, all normal, no messages | 30001=6 stop, 30002=3 TU, 30003=0 main ready, 30004=1 block circuit normal, 30005=1 braking path normal, 30006=0x0111 BU1/BU2/BU3 green, 30008 N=0, 30011=1000 braking path, 30013=5000 running path |
| `4_running_up.txt` | Running up | 30001=2 up, 30003=6 working |
| `4_running_down.txt` | Running down | 30001=3 down, 30003=6 working |
| `4_block_circuit_broken.txt` | Block circuit broken | 30004=0, 30016=0x8000 (first block circuit red) |
| `4_messages.txt` | Two new messages | 30008 N=2, 30031=0x0A01 (message: param 0x0A, type 1), 30032=0xFFFD (block-circuit id 3 for the previous message) |

## Notes
- modbus_srv does not de-duplicate messages: use `4_messages.txt` only briefly, then restore the baseline (`cycle_responses.sh` does this).
- `cycle_responses.sh` cycles all variants (5 s each) and restores the baseline at the end.
- Scenarios can be switched live: `cp 4_running_up.txt 4.txt`.
