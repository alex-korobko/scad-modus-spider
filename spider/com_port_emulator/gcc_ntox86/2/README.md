# Slave 2 - UDKU

- Device type: UDKU (`metro_udku.cpp`), device number (slave address) **2**
- Request: `02 04 00 00 00 09` - function 04, 9 registers (30001-30009)
- Response file: address, 0x04, byte count `0x12` (18 data bytes, 23-byte frame). No CRC - the emulator appends it.

## Files
| File | Scenario | Signals |
|---|---|---|
| `4.txt` / `4_baseline.txt` | Baseline: GPSTOP, TU, ready | 30001=0 GPSTOP, 30002=1 (DKSE left, escalator type 1), 30003=0xFF00 braking OK, 30005=0x3200 nominal speed 50 cm/s, current 0, 30008=0x0003 (RG2 ready + TU) |
| `4_running_up.txt` | Running up | 30001=1 GPUP, 30005=0x3232 (current speed 50), 30008=0x008B (RKP + RPV1 up + RG2 + TU) |
| `4_running_down.txt` | Running down | 30001=2 GPDOWN, 30005=0x3232, 30008=0x0087 (RKP + RPN1 down + RG2 + TU) |
| `4_block_circuit_broken.txt` | Block circuit broken | 30008=0x0101: RG2=0 (not ready), one block-circuit contact bit set (hi byte), TU=1 |
| `4_mu.txt` | Switched to MU | 30008=0x0002: bit0 (TU)=0, RG2=1 |
| `4_brake_fault.txt` | Braking fault | 30001=8 BRAK0 |

## Notes
- modbus_srv generates UDKU messages from edges between polls, so variants only produce messages when cycled after the baseline.
- `cycle_responses.sh` cycles all variants (5 s each) and restores the baseline at the end.
