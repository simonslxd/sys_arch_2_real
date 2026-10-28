# M2: State machine and loan scenario

Goal: the `LEDIG`/`UTLÅNT` state machine inside `Lab_Utstyr` and the scenario program for the loan sequence
diagram, with tests and FoM measurement #2. Spec: [m2-state-and-scenario.md](../specs/m2-state-and-scenario.md).

## Steps

- [x] Read the state machine and sequence mapping, the state and sequence flow docs, and the door and
      thermostat examples.
- [x] Check the verifier's reading of the state diagram: 2 states, 2 transitions without events, no initial
      state.
- [x] Write the spec, with every element's C++ construct and the predicted report.
- [x] `Lab_Utstyr`: `State`/`Event` enums, transition table, `handle()`, `get_state()`; the count fires the
      events (A-05).
- [x] `Lab::laan_ut`: find the student in `student_liste` (A-11, from `finnStudent`).
- [x] `scenarios/laan_ut.cpp` + `scenario_laan_ut` in `CMakeLists.txt`.
- [x] Tests: 6 new (5 state machine, 1 unregistered student); 20 of 20 pass, no warnings.
- [x] Verifier: state 22.2 %, sequence 12.5 %, class 57.5 %, all as predicted.
- [x] Update `deviations.md` (DEV-02, 03, 06, 09, 10 settled; DEV-15 new) and `requirements.md` (REQ-DOM-02,
      REQ-DOM-03 verified; A-05, A-11).
- [x] Log FoM iteration 2 (archived in `fom/history/iter-2/`); determinism check of the traced run.
- [x] The team reviews, commits and tags `iter-2`.
