# M7 spec: show how many of each item are available

**The change (team, 2026-10-29):** the dashboard clearly shows, for every item, how many units are available out of
the total, for example **"Multimeter: 1 av 2 ledig"**. The number goes down on a loan and up on a return.
Everything else stays as it is: the diagrams are not changed, and the FoM is computed as before.

**Team decisions:**
- the count is shown as *available av total*;
- the dashboard **asks the micro:bit for every item's status** when it connects;
- CSV files from iteration 6 (without counts) are **refused** with a clear message.

## Why this touches the micro:bit

Today the dashboard cannot know the counts. The class diagram has no getter for `antall_tilgjengelig` or
`antall_totalt` (DEV-16), so the protocol sends only `LEDIG` / `UTLÅNT`. The micro:bit also sends its `START` line
before the browser connects, so items nobody has touched are unknown to the dashboard.

## Model (`project/impl/`)

| Added to `Lab_Utstyr` | Why |
|---|---|
| `int get_antall_tilgjengelig() const` | the available count, for the protocol |
| `int get_antall_totalt() const` | the total, for "x av y" |

Neither getter is in the class diagram, which is unchanged by team decision. Expected class report: **2 more Extra
methods**, alignment 57.5 % → about **54.8 %** (23 of 42). The state and sequence reports are unchanged. This is
logged as a new deviation (DEV-17). DEV-16 is partly resolved: counts are sent now, but the student number still
has no getter.

## Serial protocol v2 (`spec/serial-protocol.md`)

**micro:bit → dashboard**, every line, 8 fields:

```
<seq>,<event>,<studentnummer>,<utstyr_navn>,<state>,<result>,<antall_tilgjengelig>,<antall_totalt>
```

- The two new fields describe the item named in the line, after the event.
  Example: `2,LAAN_UT,S001,Multimeter,UTLÅNT,OK,0,2`.
- New event **`STATUS`**: one line per item, in demo-data order, sent only as an answer to a request. `result` is
  always `OK`; `studentnummer` is the current selection.

**dashboard → micro:bit (new direction):** the single line `STATUS\n`. The micro:bit ignores anything else.

**Golden lines:** all 13 existing lines get the two count fields. The sequence ends with one status request, which
must produce 3 `STATUS` lines (seq 13–15). Both the C++ and the JS contract tests check the new file.

## Firmware (`firmware/`)

| Module | Change |
|---|---|
| `station/StationController` | `StationEvent` gets the two counts; new `status()` returns one `STATUS` event per item |
| `station/event_writer` | writes the two new fields |
| `station/SerialCommandReader` (**new**, PC-tested) | collects received characters into lines; the line `STATUS` → `Command::Status` |
| `station/InputDecoder` | unchanged |
| `device/InputHandler` | also reads waiting serial characters (non-blocking) each 20 ms poll and passes them to `SerialCommandReader` |
| `device/main.cpp` | on `Command::Status`, sends every line of `status()`; no LED or sound feedback |

**Risk:** reading serial input is new on this board. In M3, CODAL's event bus did not work for us (D-11). Reading
characters directly in the polling loop should avoid the bus, but the hardware test must confirm it. If it fails,
the fallback is the team's second option: the micro:bit repeats the status lines every few seconds.

## Dashboard (`dashboard/`)

| Module | Change |
|---|---|
| `js/event_parser.js` | 8 fields; event `STATUS`; counts must be whole numbers with 0 ≤ available ≤ total |
| `js/serial_link.js` | new `send(text)`; after connecting, the dashboard sends `STATUS\n` |
| `js/csv_format.js` | header gains `antall_tilgjengelig,antall_totalt`; a file with the old header is refused with *"Gammelt filformat uten antall (iterasjon 6). Lag en ny CSV-fil."* |
| `js/dashboard_view.js` | *Utstyrsstatus*: a new **Ledig** column, "1 av 2 ledig". *Utlån og returer*: a **Ledig** column after each event |
| `js/main.js` | `STATUS` lines update *Utstyrsstatus* only: not stored, not listed, and they don't change *Valgt på stasjonen* |

## New requirements

| ID | Requirement | Verified by |
|---|---|---|
| REQ-UI-02 | The dashboard shows, for every item, "*available* av *total* ledig"; it goes down on a loan and up on a return | Unit tests + browser test |
| REQ-COM-03 | When the dashboard connects, it asks the micro:bit for every item's status and shows all items | Contract test (golden `STATUS` lines) + browser test with the micro:bit |

Until they're verified, R drops from 19 of 19 to 19 of 21.

## How it will be verified

- **C++:**
  - the getters and `status()`;
  - `SerialCommandReader` (whole lines, split chunks, `\r\n`, unknown lines ignored);
  - the updated golden lines;
  - `replay_log` updated to replay `STATUS` answers.
- **JS:**
  - the parser (8 fields, `STATUS`, count rules, every golden line);
  - the CSV format (new header, old header refused with the message);
  - the logging rule (`STATUS` not stored).
- **Verifier:** the class report must show exactly the 2 new Extras (`get_antall_tilgjengelig`, `get_antall_totalt`)
  and nothing else new.
- **Hardware and browser:**
  1. Flash the board and connect: all 3 items appear at once with "x av y ledig".
  2. B, A, tilt: the counts follow.
  3. Record the CSV file and check it with `parseCsv`.
  4. Open an old iteration-6 file: it must be refused with the message.
