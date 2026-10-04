# Slave 5 - SHAVR (input/power supply status)

- Device type: SHAVR (`metro_shavr.cpp`), device number (slave address) **5**
- Request: `05 04 00 07 00 02` - function 04, registers 30008-30009 (2 registers)
- Response file: address, 0x04, byte count `0x04` (4 data bytes, 9-byte frame). No CRC - the emulator appends it.
- SHAVR is polled through three slaves: 5 (this one), 6 and 7 (power meters 1 and 2). All three must answer.

## Register layout
- 30008: hi = escalators 3/4, lo = escalators 1/2. Per escalator nibble: b0 supplied from input 1, b1 ready on input 1, b2 supplied from input 2, b3 ready on input 2.
- 30009 hi: b7 = 24 V present, b2 = machine hall door open.
- 30009 lo: b2 = 380 V input 1, b3 = 380 V input 2, b1 = fire (0 = alarm), b0 = smoke (0 = alarm), b4/b5/b6 = 220 V UPS / input 1 / input 2.

## Files
| File | Scenario | Signals |
|---|---|---|
| `4.txt` / `4_baseline.txt` | Normal | 30008=0x3333 (all escalators supplied + ready on input 1), 30009=0x807F (24 V, 380 V in1/in2, 220 V UPS/in1/in2 present, no fire, no smoke, door closed) |
| `4_input1_lost.txt` | Input 1 lost | 30008=0xCCCC (all escalators on input 2), 30009=0x807B (380 V input 1 off) |
| `4_fire.txt` | Fire alarm | 30009=0x807D (bit1 = 0) |
| `4_smoke.txt` | Smoke sensor | 30009=0x807E (bit0 = 0) |
| `4_door_open.txt` | Machine hall door open | 30009=0x847F (hi bit2 = 1) |

## Notes
- modbus_srv/spider generate messages from edges between polls; `cycle_responses.sh` cycles all variants (5 s each) and restores the baseline at the end.
