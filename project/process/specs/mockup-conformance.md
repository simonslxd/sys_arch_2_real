# Mock-up traceability and conformance

## Scope

The implementation is a browser-only mock-up in `src/`, not a C++ realization
of the class diagram. The original UML inputs remain unchanged. The class,
state, and sequence diagrams below reproduce the source design; the use-case
and activity diagrams describe the supported mock-up flows.

## Class diagram

Source: `project/diagrams/input/lab_utlaan_klassediagram(3).drawio`.
Generated draw.io diagram: [`class-design.drawio`](../../diagrams/output/class-design.drawio).

| Source element | Representation |
|---|---|
| `Lab` and its three `0..*` lists | Class diagram; the mock-up keeps these lists as `lab.utstyrsliste`, `lab.student_liste`, and `lab.utlaansliste` in `src/app.js`. |
| `Lab` operations | All five signatures appear in the class diagram. `laan_ut`, `registrer_retur`, and `finn_tilgjengelig_utstyr` support the UI flows; adding equipment and registering students are not exposed as mock-up actions. |
| `Lab_Utstyr` attributes and operations | All members appear in the class diagram. Availability and quantity updates are used by the UI flows. |
| `Student` attributes and operations | All members appear in the class diagram. `legg_til_laan` and `fjern_laan` maintain the local active-loan list. |
| `Utlaan` attributes and operations | All members appear in the class diagram. The browser creates one loan with `antall = 1` and presents its three attributes. |
| Relations and multiplicities | Aggregation, composition, and associations are shown in the class diagram with the source multiplicities and role names. |

The source spells `Student.mine_laan` as `Utlån[0..*]`, while other uses refer
to `Utlaan`. The mock-up displays the source spelling in the class diagram and
does not create a second class. The class diagram also uses `Bool` as the return
type of `Lab.finn_tilgjengelig_utstyr`; the implementation uses JavaScript
booleans for the local decision.

The browser UI does not expose `legg_til_utstyr`, `registrer_student`, or the
getter operations as actions. Its initial examples are static data, and it
supports only borrowing and returning, as scoped in the mock-up specification.
This is a UI-scope limitation, not a change to the class diagram.

## State machine

Source: `project/diagrams/input/TilstandDiagram.drawio`.
Generated draw.io diagram: [`state-lab-utstyr-design.drawio`](../../diagrams/output/state-lab-utstyr-design.drawio).

| Source element | Implementation |
|---|---|
| Initial state `LEDIG` | Every example equipment type starts with all exemplars available. |
| `LEDIG` → `UTLÅNT`, `regUtlaan()` | Borrowing calls the matching transition helper, which reduces `antall_tilgjengelig`; the displayed state is derived from whether the count is greater than zero. |
| `UTLÅNT` → `LEDIG`, `regRetur()` | Returning calls the matching transition helper, which increases `antall_tilgjengelig`; the displayed state is recalculated. |

The source state machine is binary, but a `Lab_Utstyr` entry may represent
multiple exemplars. For the overview, the documented assumption is `LEDIG`
when `antall_tilgjengelig > 0`, otherwise `UTLÅNT`. Each loan is assumed to
contain one exemplar because `laan_ut` has no count parameter.

## Sequence diagram

Source: `project/diagrams/input/Sekvens.drawio`.
Generated draw.io diagram: [`sequence-laan-ut-design.drawio`](../../diagrams/output/sequence-laan-ut-design.drawio).

| Source message | Mock-up behavior |
|---|---|
| `Lab Ansatt` → `Lab`: `Laan(Objekt, Student)` | The user submits the loan form. |
| `Lab` → `UtlaansObjekter`: `finnObjekt(Objekt)` | The selected equipment is found in `lab.utstyrsliste`. |
| `UtlaansObjekter` → `Lab`: `Objekt` | The selected equipment object is used for the availability check. |
| `Lab` → `Objekt`: `erTilgjengelig()` | The selected equipment's available count is checked. |
| `Objekt` → `Lab`: `Bool` | If unavailable, the UI shows a Norwegian message and creates no loan. |
| `Lab` → `Studenter`: `finnStudent(Student)` | If equipment is available, the selected student is found. |
| `Studenter` → `Lab`: `Student` | The found student is used to create the loan. |
| `Lab` → `Objekt`: `settStudentPaaLaan()` | A `Utlaan` is created and added to the equipment/student/lab lists. |

The availability branch is specified by the mock-up requirements. The UML
sequence source does not draw an `alt` fragment; the branch in the generated
sequence diagram makes the implemented UI guard explicit.

The source also has a dashed return labeled `Laan`, but its XML target points to
`Lab` although its position suggests a different recipient. The recipient
cannot be established reliably from the source, so this return is intentionally
not simulated or assigned to a participant. This remains an open source
question; no recipient has been assumed.

The source additionally draws a `Bool` return from `Lab` to `Lab Ansatt`, while
the class diagram declares `Lab.laan_ut(...)` to return `Utlaan`. The sequence
diagram preserves the drawn `Bool` return as source notation; the browser UI
instead displays a confirmation message and does not treat that return label as
a changed class signature.

The lifeline names `UtlaansObjekter`, `Objekt`, and `Studenter` are preserved
only as scenario labels. They are not introduced as class-diagram classes.

## Additional diagrams

- [`use-case-design.drawio`](../../diagrams/output/use-case-design.drawio) shows the
  `Lab Ansatt` use cases implemented by the page.
- [`activity-design.drawio`](../../diagrams/output/activity-design.drawio) shows the
  borrow/return paths and the unavailable-equipment branch.
- These use-case and activity diagrams are derived from the mock-up
  requirements and code; they were not present among the original UML inputs.

## Explicit mock-up assumptions

- The names in the sequence lifelines correspond to nearby class roles only
  for purposes of the UI interaction; the diagrams do not establish that they
  are synonyms.
- Each loan contains one unit (`antall = 1`).
- Equipment is selected using its unique `utstyr_navn`; no visible equipment ID
  is added.
- A return can be selected only from loans created during the current page
  session. There are no initial active loans.
- Names, student numbers, equipment labels, and counts are fictional
  placeholders. All changes are kept in memory and a refresh resets them.
- `UTLÅNT`/`LEDIG` for an equipment type follows the aggregate-availability
  rule documented above.

## Validation

- `node --check src/app.js` — passed.
- Browser interaction check — borrowing reduced the count, returning restored
  it, an attempt after all copies were borrowed showed the unavailable message
  without creating another loan, and refresh restored the initial data.
- Responsive viewport check at 390 px — document width remained 390 px.
- `tools/verify.py` was run against isolated copies of the three source
  diagrams, renamed to the verifier's supported `class.drawio`,
  `state-<class>.drawio`, and `sequence-<scenario>.drawio` patterns. It
  successfully parsed all four classes, five sequence lifelines/calls, and two
  states. Its parser warned that one class connector and one state connector
  were unattached; it does not read the standalone `get_navn()` label or the
  separate state-event labels. It also represents the actor as `Actor` because
  the visible `Lab Ansatt` label is a separate draw.io text shape. A
  parser-to-output comparison therefore reports those encoding differences:
  role-labeled relation keys and `get_navn()` on the class diagram, the detached
  start/event annotations on the state diagram, and the actor label on the
  sequence diagram. These were cross-checked against the original draw.io
  content and specification, not treated as extra or missing model elements.
  `verify.py` neither validates custom `.puml` syntax nor recognizes the
  use-case/activity diagram types.
- The C++ UML verifier is not a test of this static browser mock-up. The
  GitHub Pages workflow has not been run against GitHub, and no push or deploy
  has been made.
