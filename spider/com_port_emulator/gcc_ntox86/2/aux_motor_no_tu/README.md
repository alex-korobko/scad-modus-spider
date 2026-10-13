# UDKU (slave 2): additional (small) motor, no TU/DU

Reproduces the false `0x0006` "Останов! Тормозной путь" that modbus_srv emitted after every run on the additional motor.
Frames are taken from `logs/log_spider_no_tu_small_motor_stop_messages.txt`. Format: one byte per line, no CRC.

| File | r1 | r5 | r8 | Meaning |
|---|---|---|---|---|
| `4_stop.txt` | 0000 | 5e00 | 0402 | idle, B16=0 (additional drive), RG2 ok |
| `4_start.txt` | 000c | 5e00 | 042a | KT + RPV1 set: moving up, 30001 = 12 (additional drive up) |
| `4_running.txt` | 000c | 5e03 | 042a | moving up, speed 3 cm/s, 30001 = 12 |
| `4_running_down.txt` | 000d | 5e03 | 0426 | moving down (KT + RPN1), speed 3 cm/s, 30001 = 13 (additional drive down) |
| `4_coasting.txt` | 00ff | 5e03 | 0402 | contactors dropped, speed still 3 |

r8 low byte: bit7 RKP (0 = nominal), bit5 KT, bit4 RG (TU ready), bit3 RPV1, bit2 RPN1, bit1 RG2 (block circuit ok),
bit0 B16 (1 = main drive, 0 = additional drive).

`cycle_responses.sh [N]` copies stop -> start -> running -> coasting -> stop -> running_down -> coasting -> stop (5 s each, default 3 cycles) into `../4.txt`
and restores `../4_baseline.txt` at the end.

## Expected modbus_srv output
- Before the fix: each cycle adds `00 06` to the message block (upper id incremented) and sloginfo shows
  "unknown value in 30001 register`s low byte" on every poll with 12, 13 or 255 (the `4_coasting.txt` frame still has the 0xFF transient).
- After the fix: no new messages at all, no 255 errors and no "unknown value" for 12/13. The spider device properties window shows "ДП ПIДЙОМ" (12) / "ДП СПУСК" (13) while running. While the escalator is on the additional drive (B16 = 0) modbus_srv generates no messages (stop, TU/MU, block circuit, fault codes); the tracked state is still updated.
