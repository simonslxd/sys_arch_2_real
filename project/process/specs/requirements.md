# Requirements: micro:bit lab-lending IoT prototype

This file has three separate lists, so they are never mixed up:

1. **Requirements**: what the system must do. Each has an ID, a way to verify it and a status.
2. **Assumptions**: gaps in the models that we fill in ourselves. They are not model facts.
3. **Implementation decisions**: technical choices made to meet the requirements.

Model deviations are logged in [deviations.md](deviations.md).

`fom/measure_fom.py` reads the **Status** column: the requirement coverage *R* is the number of rows marked
`Verified` divided by the number of rows. Only mark a row `Verified` when its evidence exists (a passing test,
a report or a recorded manual check), and name that evidence in the **Evidence** column.

## Requirements

| ID | Requirement | Verified by | Status | Evidence |
|---|---|---|---|---|
| REQ-HW-01 | Right button (B) borrows the current item for the current student | Manual hardware test + controller unit test | Open | |
| REQ-HW-02 | Left button (A) returns the current student's loan of the current item | Manual hardware test + controller unit test | Open | |
| REQ-HW-03 | Tilt left selects the next student; tilt right selects the next item (wrap-around) | Manual hardware test + controller unit test | Open | |
| REQ-HW-04 | The LED matrix shows ✓ on success and ✗ on failure | Manual hardware test | Open | |
| REQ-HW-05 | The speaker plays a high beep on success and a low beep on failure | Manual hardware test | Open | |
| REQ-DOM-01 | `Lab`, `Lab_Utstyr`, `Student`, `Utlaan` are implemented as drawn in the class diagram | Class report | Verified | M1: every drawn element is implemented; class report 62.2 %, and each difference is explained in DEV-05, DEV-08, DEV-12, DEV-13 and DEV-14 |
| REQ-DOM-02 | The `LEDIG`/`UTLÅNT` state machine is implemented as drawn | State report | Open | |
| REQ-DOM-03 | The loan scenario follows the sequence diagram as closely as the model allows | Sequence report + deviation log | Open | |
| REQ-COM-01 | Every action sends one protocol line over USB serial at 115200 baud | C++ formatter test + serial monitor | Open | |
| REQ-COM-02 | The dashboard connects with WebSerial (the user clicks Connect) and receives the lines | Manual browser test | Open | |
| REQ-STO-01 | Every received event is appended to a CSV file chosen by the user | Manual test + JS CSV test | Open | |
| REQ-STO-02 | On start, the dashboard can open an existing CSV file and show its stored events | Manual test + JS test | Open | |
| REQ-UI-01 | The dashboard shows a live event list, stored history and current state per item, in Norwegian | Manual test | Open | |
| REQ-TST-01 | Unit tests for the domain (C++) and for the parser and CSV handling (JS) | Test runners | Open | |
| REQ-TST-02 | At least one benchmark | `benchmarks/` output | Open | |
| REQ-VER-01 | The verifier runs deterministically on the unchanged diagrams | `fom/measure_fom.py` | Verified | M1: two runs produced 8 byte-identical output files (3 reports, 5 `.mmd`) |
| REQ-VER-02 | The FoM is measured at 3 or more iterations and logged | `fom/fom-log.csv` | Open | |
| REQ-PRO-01 | Each iteration is one commit + tag (`iter-N`), pushed to GitHub on approval | `git log`, `git tag` | Open | |
| REQ-PRO-02 | Each module has one responsibility | Review against the module table in the architecture plan | Open | |

## Assumptions

Each one fills a gap in the models. Any of them can be changed by the team.

| ID | Assumption | Why it is needed |
|---|---|---|
| A-01 | The firmware has fixed demo data: 3 fictional students (`S001`–`S003`) and 3 items with counts | The device has no way to call `registrer_student` or `legg_til_utstyr` at runtime |
| A-02 | After a tilt, the LEDs scroll the selection's short ID (for example `S2`, `U3`), and a `VELG` event is sent | The user must see which student or item is selected |
| A-03 | Return (button A) returns the current student's loan of the current item; if there is none, the result is ✗ + low beep | `registrer_retur(utlaan)` needs a loan, and the device only knows the current selection |
| A-04 | Each loan is one unit (`antall = 1`) | `laan_ut` has no count parameter, but `Utlaan` has `antall` |
| A-05 | An item's state is `LEDIG` while `antall_tilgjengelig > 0`, otherwise `UTLÅNT` | The state machine is binary, but an item has several units |
| A-06 | High beep ≈ 1760 Hz for 150 ms; low beep ≈ 220 Hz for 300 ms | No pitch or length is specified |
| A-07 | The CSV file is chosen once per browser session | The browser asks for file permission again in each session |
| A-08 | `finn_tilgjengelig_utstyr(utstyr)` is true when `utstyr` is in this lab's `utstyrsliste` **and** `er_tilgjengelig()` | The name says "find" and the result is `Bool`; the diagram doesn't define the rule |
| A-09 | `reduser_antall_tilgjengelig` never goes below 0, and `oek_antall_tilgjengelig` never goes above `antall_totalt` | A count can't be negative or larger than the number the lab owns |
| A-10 | `laan_ut` returns `nullptr` when the item can't be lent, and `registrer_retur` ignores a loan that isn't in this lab | The diagram gives no failure case for either |

## Implementation decisions

| ID | Decision | Reason |
|---|---|---|
| D-01 | The host build, verifier, tests and firmware build run in **WSL Debian**; the dashboard runs in Chrome/Edge on Windows | The verifier needs GCC/Clang, `nm` and executables without `.exe`, so it cannot check sequence diagrams on native Windows |
| D-02 | `project/impl/` holds only the four model classes, as plain C++20 without hardware calls. The same code is built for the PC, the verifier and the micro:bit | The code on the device is then exactly the code verified against the model |
| D-03 | Firmware, dashboard, tests, benchmarks and FoM tooling live outside `project/impl/` (`firmware/`, `dashboard/`, `tests/`, `benchmarks/`, `fom/`) | They are not in the class diagram, so inside `project/impl/` they would show up as "Extra" |
| D-04 | Storage is a CSV file only, written with the File System Access API; there is no IndexedDB | The team's choice |
| D-05 | The verifier runs on byte-identical, renamed copies of the diagrams in `build/verify/` | The input file names don't match the verifier's naming patterns, and the inputs must not be edited (DEV-01) |
| D-06 | micro:bit V2 with the CODAL C++ toolchain (`arm-none-eabi-gcc`) | V2 has the speaker and enough RAM for the domain classes |
| D-07 | `fom/measure_fom.py` runs the verifier with `CPATH` set to GCC's built-in headers (`c++ -print-file-name=include`) | The pip libclang wheel lacks clang's built-in headers and stops at "'stddef.h' file not found"; the tools stay unchanged |
| D-08 | C++ unit tests use a 50-line runner (`tests/cpp/check.hpp`) instead of a test framework | No dependencies, readable by juniors; the summary line `RESULT: x of y tests passed` feeds T in the FoM |
