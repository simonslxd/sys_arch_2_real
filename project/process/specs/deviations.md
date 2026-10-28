# Model deviations log

The team decided to **keep the diagrams as drawn** and log every place where the implementation can't, or
doesn't, match them. The diagrams in `project/diagrams/input/` are never edited.

A milestone is only "done" when every deviation it touches has a status other than *Open*:

- **Accepted**: the deviation stays, and the reason is written down.
- **Resolved**: the implementation or the diagram now matches.

The sources are `lab_utlaan_klassediagram(3).drawio` (class), `TilstandDiagram.drawio` (state) and
`Sekvens.drawio` (sequence).

**Team decision (2026-10-28):** the diagrams stay unchanged for the current iteration, including the
unattached labels and connectors (DEV-05, DEV-08 to DEV-11). They may be corrected in a later iteration; any
such change gets its own commit, so its effect on the FoM can be traced.

| ID | Source element | Deviation | Effect on verification | Handling | Status |
|---|---|---|---|---|---|
| DEV-01 | All three file names | The verifier only recognises `class.drawio`, `state-<class>.drawio` and `sequence-<scenario>.drawio` | Without renaming, the verifier would skip the files | `fom/measure_fom.py` copies them byte for byte to `build/verify/` as `class.drawio`, `state-lab_utstyr.drawio` and `sequence-laan_ut.drawio` | Accepted |
| DEV-02 | Sequence lifelines `UtlaansObjekter`, `Objekt`, `Studenter`; messages `Laan(Objekt, Student)`, `finnObjekt(Objekt)`, `erTilgjengelig()`, `finnStudent(Student)`, `settStudentPaaLaan()` | These lifelines and methods are not in the class diagram | The sequence report will show Missing/Extra entries | Each message is mapped to its nearest class method in milestone 2 and listed here | Open |
| DEV-03 | `Lab_Utstyr` class box | The box lacks `- state: State` and `+ handle(event: Event): void`, which the state machine mapping requires | Implementing the state machine adds 2 Extras to the class report | Planned: implement the state machine (milestone 2), accept the 2 Extras | Open |
| DEV-04 | `Student.mine_laan: Utlån[0..*]`; `Lab.finn_tilgjengelig_utstyr(...): Bool`; `legg_til_laan(Utlaan)` / `fjern_laan(Utlaan)` | `Utlån` is spelled differently from the class `Utlaan`; `Bool` differs from `bool`; the parameters have no names | May show as Changed in the class report | Implement as `Utlaan`, `bool` and a named parameter; record the verifier's result here in milestone 1 | Open |
| DEV-05 | `+ get_navn(): string` label (drawn under `Lab_Utstyr`) | The label is not inside the class box. **The verifier does not read it**: `Lab_Utstyr` has 3 methods in `class-design.mmd` | Implementing `get_navn()` adds 1 Extra; leaving it out diverges from the drawing and from `mockup-pages.md` | To decide in milestone 1 | Open |
| DEV-06 | State `UTLÅNT` | Contains `Å`. The verifier keeps `UTLÅNT` as the label but uses the ID `UTLNT` | A renamed state would show as Changed | Try the UTF-8 identifier `UTLÅNT` first (GCC 10+ accepts it); record how the verifier compares it | Open |
| DEV-07 | — (not in any diagram) | Tilt selection, LED ✓/✗, beeps and the serial protocol are not modelled | None: the code is outside `project/impl/` | Specified in `firmware/` and the protocol spec (milestone 3) | Accepted |
| DEV-08 | Association `Utlaan` → `Lab_Utstyr` (`0..*` / `1`) | The connector is not attached at both ends. **The verifier does not read this relation**; only 4 relations appear in `class-design.mmd` | Implementing `utlaant_utstyr` as a `Lab_Utstyr*` member may count as an Extra relation | To decide in milestone 1 | Open |
| DEV-09 | State events `regUtlaan()` / `regRetur()` | They are loose text labels, not labels on the arrows. **The verifier reads both transitions with no event** and warns about it | Implemented events will show as Changed transitions | Implement the drawn events anyway (they are the source's meaning); accept the Changed entries | Open |
| DEV-10 | Initial state (start symbol → `LEDIG`) | The start arrow is not attached at both ends. **The verifier reads no initial state** | `State state_ = State::LEDIG;` may count as an Extra | Implement `LEDIG` as the start state (the source's meaning); record the verifier's result in milestone 2 | Open |
| DEV-11 | Sequence actor `Lab Ansatt`; return `Laan` | "Lab Ansatt" is a separate text box, so the verifier names the actor `Actor`. The dashed `Laan` return is read as `Objekt → Lab` | Return arrows are not compared; the actor name does not affect the call messages | None needed; recorded so the reading is known | Accepted |
| DEV-12 | Attributes `utstyrsliste`, `student_liste`, `utlaansliste`, `mine_laan`, `laanetaker`, `utlaant_utstyr` | Each is drawn **both** as an attribute and as a relation (without a role name). The mapping says a member that implements a relation is not also an attribute | One C++ member cannot match both, so expect Missing or Extra entries | Check the verifier's comparison in milestone 1 and record it | Open |

The verifier's reading of each diagram is in `build/verify/diagrams/output/*-design.mmd`, which
`fom/measure_fom.py` regenerates on every run.
