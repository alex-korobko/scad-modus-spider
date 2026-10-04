# Slave 7 - SHAVR power meter 2

- Device type: SHAVR power meter 2 (`metro_shavr.cpp`), device number (slave address) **7**
- Request: `07 04 00 47 00 20` - function 04, registers starting at 0x47, 32 registers
- Response file: address, 0x04, byte count `0x40` (64 data bytes, 69-byte frame). No CRC - the emulator appends it.

## Layout
16 x u32 (hi word first), raw values (not decoded by modbus_srv; spider scales them, see `shavr_data_block.h`):
frequency, Va, Vb, Vc, Vavg, Vab, Vbc, Vca, Vline, Ia, Ib, Ic, Ieq, P, Q, S.

## Files
| File | Scenario | Values (raw) |
|---|---|---|
| `4.txt` | Single scenario, no variants | freq 4998, Va/Vb/Vc 2205/2200/2198, Vavg 2201, Vab/Vbc/Vca 3815/3805/3810, Vline 3810, Ia/Ib/Ic 80/82/79, Ieq 80, P 5300, Q 900, S 5400 |

## Notes
- The raw values assume the scale 0.01 Hz / 0.1 V (50.00 Hz, 220.0 V). The scaling in spider has not been verified - adjust if the spider display looks wrong.
