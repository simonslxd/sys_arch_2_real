# Dashboard: the lab-lending station in the browser

Plain HTML, CSS and JavaScript (ES modules), no framework and no build step. It connects to the micro:bit over
WebSerial, reads the protocol lines (`spec/serial-protocol.md`), stores every loan and return in a CSV file on the
PC, and shows the current state, in Norwegian.

## Run it

WebSerial and file access work only in **Chrome or Edge**, and only on `https://` or `http://localhost`. Start the
small web server (in WSL, from the repository root):

```bash
python3 dashboard/serve.py
```

Open **http://localhost:8000**, then:

1. **Ny CSV-fil** (a new file) or **Åpne CSV-fil** (an earlier file: its loans and returns are shown again).
2. **Koble til micro:bit** and choose the micro:bit. This only works once a file is chosen, so nothing received
   goes unstored.

## What is stored

Every `LAAN_UT` and `RETUR`, both OK and FEIL. `START` and `VELG` only update the page. One line per event: the time
it was received, then the protocol line unchanged:

```
mottatt,seq,event,studentnummer,utstyr_navn,state,result
2026-10-28T12:00:33.466Z,2,LAAN_UT,S001,Oscilloskop,UTLÅNT,OK
```

The file is UTF-8. To open it in Excel, use *Data → From Text/CSV* and choose UTF-8, or `UTLÅNT` shows garbled.

## Modules (one responsibility each)

| File | Responsibility | Tested |
|---|---|---|
| `js/serial_link.js` | WebSerial: choose and open the port (115200 baud), read UTF-8 text, hand out complete lines (`LineBuffer`) | `LineBuffer`: `tests/js/line_buffer.test.mjs`; the connection: by hand |
| `js/event_parser.js` | One protocol line → event object with the protocol's field names, or an error saying why; counts lines missing between two `seq` values | `tests/js/event_parser.test.mjs`, including the 13 golden lines |
| `js/csv_format.js` | Which events are stored; event ↔ CSV line; reading a whole file | `tests/js/csv_format.test.mjs` |
| `js/csv_store.js` | File System Access: choose a new or existing file, read it, append lines one after another | by hand |
| `js/dashboard_view.js` | Everything on the page: status, buttons, selection, item states, loans and returns, problems | by hand |
| `js/main.js` | Wiring: file → page; line → parser → page, and → CSV for loans and returns. No logic of its own | by hand |
| `serve.py` | The web server, without browser caching (so a reload always gets the current JS) | — |

`package.json` only tells Node that `js/*.js` are ES modules, so the tests can import them. There are no
dependencies.

## Tests

```bash
node --test tests/js/        # in WSL, from the repository root
```
