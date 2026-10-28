# SRP review (REQ-PRO-02)

Every module of the prototype, checked against the one responsibility it claims: in its header comment, its
README and the architecture's module table. Done in M6 (2026-10-28) by reading each file.

**Verdicts:** ✅ one responsibility; ⚠️ one responsibility, with a remark; ✗ more than one (none found).

## Domain model: `project/impl/`

The responsibilities come from the class diagram, not from us.

| Module | Responsibility | Verdict | Remark |
|---|---|---|---|
| `Lab` | the lab's equipment, students and loans; lending and returning | ✅ | as drawn |
| `Lab_Utstyr` | one kind of equipment: its counts and its state machine | ✅ | the state machine belongs to this class in the state diagram |
| `Student` | a student and the loans they hold | ✅ | |
| `Utlaan` | one loan: who, what, how many | ✅ | |

## Firmware: `firmware/`

| Module | Responsibility | Verdict | Remark |
|---|---|---|---|
| `station/StationController` | run a Command on the domain model, keep the current selection | ⚠️ | it also numbers the events (`seq`) and turns the state into text. Both describe the result of a Command, so they stay; a separate "event factory" would add a class for two lines |
| `station/InputDecoder` | raw readings → Commands | ✅ | |
| `station/event_writer` | StationEvent → one protocol line | ✅ | |
| `station/demo_data.hpp` | the demo data | ✅ | |
| `device/InputHandler` | read buttons and accelerometer every 20 ms | ✅ | |
| `device/FeedbackOutput` | LEDs and speaker | ✅ | |
| `device/main.cpp` | wiring | ⚠️ | it also picks the feedback per Command and builds the `S2` / `U3` text (`show_feedback`). The header comment claimed "only wires"; **corrected in M6** |

## Dashboard: `dashboard/`

| Module | Responsibility | Verdict | Remark |
|---|---|---|---|
| `js/serial_link.js` | deliver complete lines from the micro:bit | ✅ | `SerialLink` (WebSerial) and `LineBuffer` (chunks → lines) are the two halves of that job; `LineBuffer` is separate so it can be tested |
| `js/event_parser.js` | the protocol's rules for one line | ⚠️ | it also has `missingLines` (gaps in `seq`). Both apply `spec/serial-protocol.md`, so they belong together |
| `js/csv_format.js` | the CSV file's format, and which events go in it | ✅ | |
| `js/csv_store.js` | file access | ✅ | |
| `js/dashboard_view.js` | everything on the page | ✅ | the largest module (100 lines), but every function only draws |
| `js/main.js` | wiring | ⚠️ | it also keeps the session's state (`connected`, last `seq`) and chooses between "store and list" and "only update the page". The rules it applies are in the tested modules. The header comment claimed "no logic of its own"; **corrected in M6** |
| `serve.py` | serve the dashboard without caching | ✅ | |

## Tools and tests

| Module | Responsibility | Verdict | Remark |
|---|---|---|---|
| `tests/cpp/check.hpp` | run tests, report results | ✅ | |
| `tests/hardware/replay_log.cpp` | check a recorded serial log against the station code | ✅ | |
| `benchmarks/lending_benchmark.cpp` | time loan + return | ✅ | |
| `fom/measure_fom.py` | measure and log the FoM | ⚠️ | one job, but it has many steps (verifier, C++ and JS tests, requirements, benchmark, log, archive, progress page). Each step is its own function. Split it if it grows further |

## Result

No module has two unrelated responsibilities. Two header comments overstated how little their module does; both are
corrected. REQ-PRO-02 is verified.
