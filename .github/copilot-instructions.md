# Copilot instructions

## Repository layout and architecture

- `src/` is the dependency-free Norwegian browser mock-up. `index.html` provides
  the page structure, `styles.css` its responsive presentation, and `app.js`
  the example data and in-memory interactions.
- `.github/workflows/deploy-pages.yml` publishes only `src/` to GitHub Pages
  when `main` receives a push. Do not push or trigger a deployment without
  explicit user approval.
- `project/diagrams/input/` contains the authoritative, hand-drawn UML inputs.
  Read these files but do not edit them. `project/impl/` is a separate C++ UML
  implementation target; `tools/verify.py` converts its input diagrams and
  checks C++ implementations. The six `examples/` projects demonstrate that
  verifier and are not the browser mock-up.
- `project/diagrams/output/` contains editable `.drawio` diagrams for the
  mock-up. The class, state, and sequence designs in
  `project/diagrams/input/` remain authoritative where they cover the same
  elements.

## Commands

- Preview the mock-up from the repository root:
  `python3 -m http.server 8000 --directory src`
- Check JavaScript syntax:
  `node --check src/app.js`
- Verify the C++ UML project (not the browser mock-up):
  `.venv/bin/python tools/verify.py project`
- Build a C++ example with CMake, for example:
  `cmake -S examples/1-class-simple/impl -B /tmp/oblig1-build && cmake --build /tmp/oblig1-build`
- There is no configured browser test runner, unit-test command, or linter.
  Manually exercise borrowing, returning, unavailable equipment, and page
  refresh in the browser. There is no single-test selector.

## Source fidelity and implementation conventions

- Preserve the UML names and member spelling: `Lab`, `Lab_Utstyr`, `Student`,
  `Utlaan`, and the exact operation names. The class diagram spells
  `Student.mine_laan` as `Utlån[0..*]` while its other references use `Utlaan`;
  show and document that discrepancy rather than treating it as a second
  domain class.
- In the sequence diagram, `UtlaansObjekter`, `Objekt`, and `Studenter` are
  lifeline labels, not additional class-diagram classes. The dashed `Laan`
  return has a conflicting/unclear target in the source XML; do not invent its
  recipient. Keep assumptions and unresolved source differences visible in
  `project/process/specs/mockup-pages.md` and the conformance report.
- The mock-up uses plain JavaScript objects with the diagram's field names.
  Its only data source is the initial example data in `src/app.js`; do not add
  a backend, framework, or persistent browser storage. Refresh must restore
  the initial examples.
- Keep the model's supported actions limited to borrowing and returning.
  Preserve the source sequence order and the two equipment states
  `LEDIG`/`UTLÅNT`. The `antall_tilgjengelig > 0` rule is an explicit display
  assumption for equipment types with multiple copies, not a change to the UML.
- Keep the Pages artifact rooted at `src/`; do not deploy repository process
  files, UML inputs, or C++ examples.
