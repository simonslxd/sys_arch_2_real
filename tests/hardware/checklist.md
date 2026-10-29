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

## Dashboard check (M4), 2026-10-28

1. Serve the dashboard (`dashboard/README.md`) and open http://localhost:8000 in Chrome or Edge.
2. *Koble til micro:bit* → choose the micro:bit → the status turns green.
3. Give a few inputs (B, tilt left, A, tilt right, B): each adds a row at the top of *Hendelser*, and *Valgt på
   stasjonen* and *Utstyrsstatus* follow. *Avviste linjer* stays hidden.
4. *Koble fra* → the status shows *Frakoblet.*

| Requirement | Evidence | Result |
|---|---|---|
| REQ-COM-02 WebSerial connection, lines received | tester's report: "It works" | Pass (observed) |

## CSV storage check (M5), 2026-10-28

1. Serve the dashboard (`python3 dashboard/serve.py`), open http://localhost:8000, choose **Ny CSV-fil**.
2. Connect, give B, tilt left, A, tilt right, B, A; disconnect.
3. Reload the page (F5), choose **Åpne CSV-fil** with the same file: the loans and returns come back (source *fil*).
4. Check the file: `parseCsv` must read every line, and it must hold only `LAAN_UT` and `RETUR`.

| Run | File | Rows | Rejected | Notes |
|---|---|--:|--:|---|
| 1 | [logs/2026-10-28-dashboard-run1.csv](logs/2026-10-28-dashboard-run1.csv) | 24 | 0 | Before the team decided to store only loans and returns, so it also holds `VELG`. Includes the first successful `RETUR` on hardware (seq 68). **seq 86 and 87 are missing** (between 11:51:07 and 11:51:13); whether the dashboard was connected then is unknown |
| 2 | [logs/2026-10-28-dashboard-run2.csv](logs/2026-10-28-dashboard-run2.csv) | 4 | 0 | Exactly the 4 loans/returns given (B, A, B, A), with the expected results and states; the gaps in seq (3, 5) are the unlogged tilts. Reload and reopen worked |

The missing lines of run 1 are not explained. Since run 2, the dashboard warns under *Avviste og manglende linjer*
when `seq` jumps while connected (D-16). No warning was reported in run 2.

| Requirement | Evidence | Result |
|---|---|---|
| REQ-STO-01 loans and returns stored | run 2 file | Pass |
| REQ-STO-02 history shown after reload | runs 1 and 2 (tester: "it worked") | Pass (observed) |
| REQ-UI-01 Norwegian dashboard: list, selection, item states | runs 1 and 2 (tester: "it worked") | Pass (observed) |

## Count check (M7), 2026-10-29: protocol version 2

1. Flash the iteration-7 firmware. From PowerShell, send `STATUS` on COM3: the board must answer with one `STATUS`
   line per item.
2. Serve the dashboard, **Ny CSV-fil**, **Koble til micro:bit**: *Utstyrsstatus* must list all 3 items with
   "x av y ledig" before any input.
3. Give loans and returns (B, A, tilts): the *Ledig* counts follow.
4. Reload, open the file: the rows come back with their counts. Opening an iteration-6 file must give *"Gammelt
   filformat uten antall (iterasjon 6). Lag en ny CSV-fil."*
5. Check the file: `parseCsv` reads every line, and every count change follows the rules (a successful loan −1, a
   successful return +1, a refused one 0; `LEDIG` exactly when the count is above 0).

| Step | Result |
|---|---|
| 1. `STATUS` over COM3 (PowerShell) | `1,STATUS,S001,Multimeter,LEDIG,OK,2,2`, `2,STATUS,S001,Oscilloskop,LEDIG,OK,1,1`, `3,STATUS,S001,Loddestasjon,LEDIG,OK,3,3`: reading serial input works on the board, so the fallback (repeated status) is not needed |
| 2–4. Browser test | Tester: "done. looks good" |
| 5. File check | [logs/2026-10-29-dashboard-m7.csv](logs/2026-10-29-dashboard-m7.csv): 14 rows (all `LAAN_UT`/`RETUR`), 0 rejected; 11 of 11 count changes follow the rules, 0 wrong |

| Requirement | Evidence | Result |
|---|---|---|
| REQ-UI-02 "x av y ledig", following loans and returns | step 5 file check; browser test | Pass |
| REQ-COM-03 all items shown on connect | step 1; browser test | Pass |

Note: before this flash, the old firmware sent about 40 `VELG` lines within a few seconds (students cycling). That
fits a board being moved; whether it was lying still was not confirmed.
