# M5 spec: CSV storage

Sources: [requirements.md](requirements.md) (REQ-STO-01, REQ-STO-02, REQ-UI-01, REQ-TST-01, A-07, A-14, A-15,
D-04, D-15, D-16) and the serial protocol model [spec/serial-protocol.md](../../../spec/serial-protocol.md).

## Team decisions during M5 (2026-10-28)

After the first browser test, the team changed two requirements:

1. **Store only loans and returns:** every `LAAN_UT` and `RETUR`, both OK and FEIL. `START` and `VELG` are not
   stored and not listed. They still update *Valgt på stasjonen* and *Utstyrsstatus*.
2. **Show the protocol's event names only:** `LAAN_UT`, `RETUR`, without Norwegian labels.

REQ-STO-01 and REQ-UI-01 were reworded to match, and the old wording is kept in the table.

## File format (D-15)

UTF-8, one header line, then one line per stored event: the receive time (ISO 8601, UTC, from the PC's clock),
then the protocol line unchanged.

```
mottatt,seq,event,studentnummer,utstyr_navn,state,result
2026-10-28T12:00:33.466Z,2,LAAN_UT,S001,Oscilloskop,UTLÅNT,OK
```

Reading uses the same `parseEventLine` as live lines. Invalid lines are listed with their line number, and valid
ones are kept. A file that doesn't start with the header is refused, so the dashboard never writes into an
unrelated CSV file.

## Behaviour

| Step | What happens |
|---|---|
| *Ny CSV-fil* | The browser's save dialog; the header line is written. |
| *Åpne CSV-fil* | The browser's open dialog plus write permission; the file is read and its rows are shown (source *fil*); selection and item states follow the last rows. |
| *Koble til micro:bit* | Only after a file is chosen (A-14). |
| a line arrives | Parsed. If `seq` jumped since the previous line, a warning shows how many lines are missing (D-16). A `LAAN_UT`/`RETUR` is appended to the file (writes run one after another, in order) and listed (source *live*). Every line updates the selection and the item's state. |
| a write fails | The event is still shown, and the storage status turns red with the reason. |

## Modules

`csv_format.js` (pure, Node-tested) and `csv_store.js` (File System Access, browser only); see
[dashboard/README.md](../../../dashboard/README.md).

## Results

- JS tests: 16 of 16 (7 new: CSV format, round trip of the golden lines, logging rule, `seq` gaps).
- Browser runs with the micro:bit: run 2's file holds exactly the 4 loans/returns given, 0 rejected; reload and
  reopen showed the stored history (tester: "it worked"). See
  [tests/hardware/checklist.md](../../../tests/hardware/checklist.md).
- Found and fixed: with the plain Python web server, Chrome reused old JS modules after an update, and the page
  stopped working. Fixed by `dashboard/serve.py`, which disables caching (D-14).
- Open: seq 86 and 87 are missing from run 1's file, unexplained. The new gap warning reported nothing in run 2.
