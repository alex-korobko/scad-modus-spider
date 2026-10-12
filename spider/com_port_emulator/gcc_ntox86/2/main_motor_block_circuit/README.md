# UDKU (slave 2): main motor, block circuit, regression

Frames derived from `logs/log2.txt`. The stop message must still appear after the fix. Format: one byte per line, no CRC.
Every frame has r2 010b, r3/r4 0000 04ba, r6/r7 0001 8ab7.

| File | r1 | r5 | r8 | r9 | Meaning |
|---|---|---|---|---|---|
| `4_idle.txt` | 0000 | 0000 | 0093 | 0000 | idle, RG=1 (TU ready), RKP=1, B16=1 |
| `4_starting.txt` | 00ff | 002c | 00b7 | 0000 | starting, RPN1 (down), RKP=1 |
| `4_running.txt` | 0002 | 0060 | 0027 | 0000 | nominal speed: RKP=0, RG=0, RPN1 |
| `4_braking.txt` | 00ff | 0048 | 0081 | 0100 | contactors dropped, block circuit lost (RG2=0) |
| `4_stopped.txt` | 0000 | 0000 | 0081 | 0100 | stopped |

`cycle_responses.sh [N]` runs idle -> starting -> running -> braking -> stopped (5 s each, default 3 cycles) via `../4.txt`
and restores the baseline.

## Expected per cycle (after the fix)
- exactly one `0x0006`, at running -> braking (contactor bits drop; stopped is taken from RPV1/RPN1 only);
- `0x0110` (circuit ВБП+ВБЛ not ready) at braking;
- `0x0015` (TU DOWN succeeded) at starting -> running (RKP 1->0, RG=1 when stopped);
- no `0x12`/`0x13` during the run;
- no additional-drive messages (`0x19`..`0x1B`).
