# M4: Dashboard, live view

Goal: a Norwegian browser dashboard that connects to the micro:bit over WebSerial and shows its events live.
Spec: [m4-dashboard-live.md](../specs/m4-dashboard-live.md).

## Steps

- [x] `.gitattributes`: shell scripts keep LF line endings in every checkout (a fresh Windows clone broke
      `firmware/build.sh`).
- [x] `dashboard/`: `index.html`, `styles.css`, `js/serial_link.js`, `js/event_parser.js`, `js/dashboard_view.js`,
      `js/main.js`, `package.json` (`"type": "module"` only), README.
- [x] `tests/js/`: 7 tests with `node --test` (golden lines, field names, broken lines, line buffering); 7 of 7 pass.
- [x] `fom/measure_fom.py`: T counts the C++ and JS tests together (41 of 41).
- [x] Served on `localhost:8000`; tested in the browser with the micro:bit. The tester reported "It works".
- [x] `requirements.md`: REQ-COM-02 verified; D-13, D-14. Hardware checklist extended.
- [x] Log FoM iteration 4.
- [x] The team reviews, commits and tags `iter-4`.
