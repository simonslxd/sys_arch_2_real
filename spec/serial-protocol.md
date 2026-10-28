# Serial protocol: micro:bit → dashboard

The micro:bit station sends one line per event over USB serial. The dashboard (WebSerial) reads the lines, shows
them and stores them in a CSV file. This document is the **model** of that interface. The firmware's
`event_writer` (C++) and the dashboard's `event_parser` (JS, milestone 4) are both tested against the same
golden lines in [serial-protocol-examples.txt](serial-protocol-examples.txt).

## Connection

| | |
|---|---|
| Port | the micro:bit's USB serial port (CDC) |
| Speed | 115200 baud, 8 data bits, no parity, 1 stop bit (CODAL's default) |
| Direction | micro:bit → dashboard only |
| Encoding | UTF-8 (`UTLÅNT` contains `Å`) |
| Line end | `\n` |

## Line format

```
<seq>,<event>,<studentnummer>,<utstyr_navn>,<state>,<result>
```

| Field | Values | Meaning |
|---|---|---|
| `seq` | 0, 1, 2, … | Counts the events since the micro:bit started. A gap means lost lines; a reset back to 0 means a restart |
| `event` | `START`, `LAAN_UT`, `RETUR`, `VELG` | What happened (see below) |
| `studentnummer` | e.g. `S001` | The **selected** student after the event |
| `utstyr_navn` | e.g. `Multimeter` | The **selected** item after the event |
| `state` | `LEDIG`, `UTLÅNT` | The selected item's state after the event (the state machine of `Lab_Utstyr`) |
| `result` | `OK`, `FEIL` | Whether the action succeeded. `START` and `VELG` are always `OK` |

| `event` | Sent when | Domain call |
|---|---|---|
| `START` | the micro:bit has started (`seq` is 0) | — |
| `LAAN_UT` | right button (B): borrow | `Lab::laan_ut(student, utstyr)` |
| `RETUR` | left button (A): return | `Lab::registrer_retur(utlaan)` for the selected student's loan of the selected item (A-03) |
| `VELG` | tilt left (next student) or tilt right (next item) | — |

Rules:

- Fields never contain commas or line breaks. The demo data is chosen so (A-01).
- The line contains no timestamp: the micro:bit has no clock. The dashboard adds the time when it receives the
  line (milestone 5).
- The available count is not sent: the class diagram has no getter for `antall_tilgjengelig`, and the state
  says what the dashboard needs (DEV-16).

## Golden lines

[serial-protocol-examples.txt](serial-protocol-examples.txt) holds the exact lines the station must send for one
fixed sequence of inputs, starting from the demo data. The C++ test `tests/cpp/test_event_writer.cpp` replays
that sequence and compares line by line. The JS parser test (milestone 4) must accept every one of these lines.
