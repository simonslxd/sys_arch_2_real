# M7: Show how many of each item are available

Goal: the dashboard shows "x av y ledig" for every item, live. Spec:
[m7-equipment-counts.md](../specs/m7-equipment-counts.md).

## Steps

- [x] `requirements.md`: add REQ-UI-02 and REQ-COM-03; `deviations.md`: DEV-17, DEV-16 updated.
- [x] Model: `Lab_Utstyr::get_antall_tilgjengelig()`, `get_antall_totalt()` + unit tests.
- [x] Protocol v2: `spec/serial-protocol.md` and the golden lines (8 fields, `STATUS` block at the end).
- [x] Firmware, station: counts in `StationEvent`, `status()`, `event_writer`, new `SerialCommandReader` + tests;
      `replay_log` replays `STATUS`.
- [x] Firmware, device: `InputHandler` reads serial input; `main.cpp` answers `STATUS`. Build with 0 warnings
      (counted after the build).
- [x] Dashboard: parser, `serial_link.send`, CSV header and old-format message, *Ledig* columns, `STATUS` handling
      + tests.
- [x] Run the verifier: the class report shows exactly the 2 new Extras.
- [x] Hardware and browser test (the spec's 4 steps); save the CSV and the result in `tests/hardware/`.
- [x] Update the READMEs, the checklist and the final conformance report (sections 2–4).
- [x] Log FoM iteration 7.
- [x] The team reviews, commits and tags `iter-7`.
