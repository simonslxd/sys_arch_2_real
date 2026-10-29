# Serial protocol: micro:bit ↔ dashboard (version 2)

The micro:bit station sends one line per event over USB serial. The dashboard (WebSerial) reads the lines, shows
them and stores the loans and returns in a CSV file. Since version 2 (iteration 7), the dashboard can also ask the
micro:bit for the status of every item. This document is the **model** of that interface. The firmware's
`event_writer` and `SerialCommandReader` (C++) and the dashboard's `event_parser` (JS) are all tested against the
same golden lines in [serial-protocol-examples.txt](serial-protocol-examples.txt).

## Connection

| | |
|---|---|
| Port | the micro:bit's USB serial port (CDC) |
| Speed | 115200 baud, 8 data bits, no parity, 1 stop bit (CODAL's default) |
| Direction | micro:bit → dashboard: event lines. Dashboard → micro:bit: only the request `STATUS` (version 2) |
| Encoding | UTF-8 (`UTLÅNT` contains `Å`) |
| Line end | `\n` (a `\r` before it is ignored) |

## micro:bit → dashboard: event lines

```
<seq>,<event>,<studentnummer>,<utstyr_navn>,<state>,<result>,<antall_tilgjengelig>,<antall_totalt>
```

| Field | Values | Meaning |
|---|---|---|
| `seq` | 0, 1, 2, … | Counts the lines since the micro:bit started. A gap means lost lines; a reset back to 0 means a restart |
| `event` | `START`, `LAAN_UT`, `RETUR`, `VELG`, `STATUS` | What happened (see below) |
| `studentnummer` | e.g. `S001` | The **selected** student after the event |
| `utstyr_navn` | e.g. `Multimeter` | The item the line is about: the **selected** item, or for `STATUS` each item in turn |
| `state` | `LEDIG`, `UTLÅNT` | That item's state after the event (the state machine of `Lab_Utstyr`) |
| `result` | `OK`, `FEIL` | Whether the action succeeded. `START`, `VELG` and `STATUS` are always `OK` |
| `antall_tilgjengelig` | 0 … `antall_totalt` | That item's available units after the event (version 2) |
| `antall_totalt` | 0, 1, 2, … | That item's total units (version 2) |

| `event` | Sent when | Domain call |
|---|---|---|
| `START` | the micro:bit has started (`seq` is 0) | — |
| `LAAN_UT` | right button (B): borrow | `Lab::laan_ut(student, utstyr)` |
| `RETUR` | left button (A): return | `Lab::registrer_retur(utlaan)` for the selected student's loan of the selected item (A-03) |
| `VELG` | tilt left (next student) or tilt right (next item) | — |
| `STATUS` | the dashboard sent `STATUS`: one line per item, in demo-data order (version 2) | `Lab_Utstyr::get_antall_tilgjengelig()`, `get_antall_totalt()` |

## Dashboard → micro:bit: the status request (version 2)

The only line the micro:bit understands is `STATUS` followed by `\n`. It answers with one `STATUS` line per item.
Any other line is ignored. The dashboard sends it right after connecting, because the micro:bit's `START` line
went out before the dashboard was listening.

## Rules

- Fields never contain commas or line breaks. The demo data is chosen so (A-01).
- The line contains no timestamp: the micro:bit has no clock. The dashboard adds the time when it receives the
  line (A-15).
- The student number comes from the station's own list, because the class diagram has no getter for it (DEV-16).
  The counts come from the getters added in iteration 7 (DEV-17).

## Golden lines

[serial-protocol-examples.txt](serial-protocol-examples.txt) holds the exact lines the station must send for one
fixed sequence of inputs, starting from the demo data and ending with one status request. The C++ test
`tests/cpp/test_event_writer.cpp` replays that sequence and compares line by line. The JS parser test must accept
every one of these lines.
