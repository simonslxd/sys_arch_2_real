# M4 spec: dashboard, live view

Sources: the approved architecture (dashboard modules), [requirements.md](requirements.md) (REQ-COM-02,
REQ-UI-01, REQ-TST-01) and the serial protocol model [spec/serial-protocol.md](../../../spec/serial-protocol.md).
Storage (CSV) is milestone 5 and is not part of this spec.

## What the page shows (Norwegian)

| Section | Content | From |
|---|---|---|
| Tilkobling | status (frakoblet / tilkoblet / feil); buttons *Koble til micro:bit* and *Koble fra* | `SerialLink` |
| Valgt på stasjonen | the selected `studentnummer` and `utstyr_navn` | the latest event (every event carries the selection) |
| Utstyrsstatus (`Lab_Utstyr`) | one row per item seen so far: `utstyr_navn`, state `LEDIG` / `UTLÅNT` | each event updates the row of the item it names |
| Hendelser | every event, newest first: all 6 protocol fields, plus a Norwegian label for the event | each event |
| Avviste linjer | lines that break the protocol, with the reason; hidden while there are none | `parseEventLine` errors |

Model names (`Lab_Utstyr`, `studentnummer`, `utstyr_navn`, `LEDIG`, `UTLÅNT`) are shown unchanged; Norwegian text
explains them. The page shows only what the protocol carries: no counts (DEV-16), no times (they arrive with the
CSV storage in M5).

## Modules

See [dashboard/README.md](../../../dashboard/README.md). The two parts with logic, `event_parser.js` and
`LineBuffer`, use no browser features and are tested with Node's built-in test runner (`node --test`, no npm
packages). `event_parser.js` is tested against the same golden lines as the firmware's C++ `event_writer`, which
makes them a contract test of the interface: the firmware must produce those lines, and the dashboard must
accept them.

## Results

- JS tests: 7 of 7 pass (golden lines, field names, line ends, 7 kinds of broken line, line buffering).
- Browser test with the micro:bit (Chrome/Edge, `http://localhost:8000`): connected through WebSerial, events
  appeared live as the board was used. The tester reported "It works" (REQ-COM-02).
- The FoM's T now counts the C++ and JS tests together: 41 of 41.
