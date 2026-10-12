# UDKU (slave 2): additional (small) motor, no TU/DU

Reproduces the false `0x0006` "Останов! Тормозной путь" that modbus_srv emitted after every run on the additional motor.
Frames are taken from `logs/log_spider_no_tu_small_motor_stop_messages.txt`. Format: one byte per line, no CRC.

| File | r1 | r5 | r8 | Meaning |
|---|---|---|---|---|
| `4_stop.txt` | 0000 | 5e00 | 0402 | idle, B16=0 (additional drive), RG2 ok |
| `4_start.txt` | 0000 | 5e00 | 042a | KT + RPV1 set: moving |
| `4_running.txt` | 00ff | 5e03 | 042a | moving, speed 3 cm/s, R1=0xFF transient |
| `4_coasting.txt` | 00ff | 5e03 | 0402 | contactors dropped, speed still 3 |

r8 low byte: bit7 RKP (0 = nominal), bit5 KT, bit4 RG (TU ready), bit3 RPV1, bit2 RPN1, bit1 RG2 (block circuit ok),
bit0 B16 (1 = main drive, 0 = additional drive).

`cycle_responses.sh [N]` copies stop -> start -> running -> coasting -> stop (5 s each, default 3 cycles) into `../4.txt`
and restores `../4_baseline.txt` at the end.

## Expected modbus_srv output
- Before the fix: each cycle adds `00 06` to the message block (upper id incremented) and sloginfo shows
  "unknown value in 30001 register`s low byte : 255" on every 0xFF poll.
- After the fix: no 255 errors. The additional drive uses the same messages as the main drive:
  - `0x0006` (stop, braking path) once per cycle at coasting -> stop (contactors off and speed 0). It is sent for every stop, TU or MU, so it still appears here. The braking path r3/r4 is constant (`04ba`) in these frames, so the value shown may be stale.
  - `0x0014`/`0x0015` (TU command UP/DOWN succeeded) at stop -> start, only if the escalator was on TU (RG = 1 while stopped). RG = 0 in these frames, so they do not appear.
