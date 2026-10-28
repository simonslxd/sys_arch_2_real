# Hardware check: micro:bit station

Covers REQ-HW-01 to REQ-HW-05 and REQ-COM-01, the parts the PC tests cannot reach: real buttons, the real
accelerometer, the LEDs, the speaker and the USB serial port.

## How to run it

1. Build and flash the firmware (`firmware/README.md`).
2. Record the serial port from the moment the board starts (COM3 on the development PC, 115200 baud, UTF-8). Save
   the protocol lines to `tests/hardware/logs/<date>-run<N>.txt`.
3. Hold the board flat, USB plug pointing away from you, and give the 12 inputs below. Wait for each response
   before the next input.
4. Note what the LEDs and the speaker did.
5. Check the log, in WSL from the repository root:
   `cmake -S tests/cpp -B build/tests && cmake --build build/tests && ./build/tests/replay_log tests/hardware/logs/<file>`

The 12 inputs are those of `spec/serial-protocol-examples.txt`. If they are given exactly, the log equals that
golden file. `replay_log` doesn't need that: it works out from the log which inputs the board received, replays them
through the PC-tested `StationController`, and checks that every recorded line is exactly what that code produces.

| # | Input | Expected feedback |
|--:|---|---|
| 1–3 | B, B, B | ✓ high, ✓ high, ✗ low |
| 4 | tilt left | `S2` |
| 5 | A | ✗ low |
| 6–7 | tilt left, tilt left | `S3`, `S1` |
| 8 | A | ✓ high |
| 9 | tilt right | `U2` |
| 10 | B | ✓ high |
| 11–12 | tilt right, tilt right | `U3`, `U1` |

## Results, 2026-10-28 (micro:bit V2.2, firmware of iteration 3)

| Run | Log | Equal to golden file | `replay_log` | Notes |
|---|---|---|---|---|
| 1 | [logs/2026-10-28-run1.txt](logs/2026-10-28-run1.txt) | 11 of 13 | **13 of 13** | Step 8: B was pressed instead of A (confirmed by the tester); lines 8 and 12 follow from that |
| 2 | [logs/2026-10-28-run2.txt](logs/2026-10-28-run2.txt) | 5 of 13 | **13 of 13** | From step 5 the inputs differ from the table (an extra tilt left after step 4; later the left/right tilts and A/B look mirrored, as when the board is held turned around) |

`replay_log` was also checked to fail on a log with one changed field (it reported the line and exit code 1).

| Requirement | Evidence | Result |
|---|---|---|
| REQ-HW-01 B borrows | run 1 lines 1–3, 10 (`LAAN_UT`, OK/OK/FEIL/OK); run 2 lines 1–3 | Pass |
| REQ-HW-02 A returns | run 1 line 5; run 2 lines 6, 9, 11 (`RETUR`) | Pass. Every hardware `RETUR` had no matching loan (`FEIL`); a successful return is covered by the unit tests and by the replay matching |
| REQ-HW-03 tilt selects | run 1 lines 4, 6–7 (S002, S003, S001 with wrap-around), 9, 11–12 (Oscilloskop, Loddestasjon, Multimeter with wrap-around) | Pass |
| REQ-HW-04 ✓ / ✗ on LEDs | tester's observation: "it went well" | Pass (observed) |
| REQ-HW-05 high / low beep | tester's observation: "it went well" | Pass (observed) |
| REQ-COM-01 one line per action, 115200 baud | 2 × 13 lines, one per input, all valid protocol lines | Pass |
