# Dashboard: the lab-lending station in the browser

Plain HTML, CSS and JavaScript (ES modules), no framework and no build step. It connects to the micro:bit over
WebSerial, reads the protocol lines (`spec/serial-protocol.md`) and shows them, in Norwegian.

## Run it

WebSerial works only in **Chrome or Edge**, and only on `https://` or `http://localhost`. ES modules don't load from
a plain file, so start a small local web server (in WSL, from the repository root):

```bash
python3 -m http.server 8000 --bind 127.0.0.1 --directory dashboard
```

Open **http://localhost:8000**, click **Koble til micro:bit** and choose the micro:bit in the list.

## Modules (one responsibility each)

| File | Responsibility | Tested |
|---|---|---|
| `js/serial_link.js` | WebSerial: choose and open the port (115200 baud), read UTF-8 text, hand out complete lines (`LineBuffer`) | `LineBuffer`: `tests/js/line_buffer.test.mjs`; the connection: by hand |
| `js/event_parser.js` | One protocol line → event object with the protocol's field names, or an error saying why | `tests/js/event_parser.test.mjs`, including the 13 golden lines |
| `js/dashboard_view.js` | Everything on the page: status, current selection, item states, events, rejected lines | by hand |
| `js/main.js` | Wiring: line → parser → view. No logic of its own | by hand |

`package.json` only tells Node that `js/*.js` are ES modules, so the tests can import them. There are no
dependencies.

## Tests

```bash
node --test tests/js/        # in WSL, from the repository root
```
