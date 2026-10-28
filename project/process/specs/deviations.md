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
| DEV-04 | `Student.mine_laan: Utlån[0..*]`; `Lab.finn_tilgjengelig_utstyr(...): Bool`; `legg_til_laan(Utlaan)` / `fjern_laan(Utlaan)` | `Utlån` is spelled differently from the class `Utlaan`; `Bool` differs from `bool`; the parameters have no names | **M1 result:** `Bool` = `bool` and the unnamed parameters are Identical (names are not compared). `Utlån` is not compared (see DEV-14) | Implemented as `Utlaan`, `bool` and `utlaan` | Resolved |
| DEV-05 | `+ get_navn(): string` label (drawn under `Lab_Utstyr`) | The label is not inside the class box. **The verifier does not read it**: `Lab_Utstyr` has 3 methods in `class-design.mmd` | **M1 result:** 1 Extra method | Implemented, because it is drawn. A diagram fix (moving it into the box) is a candidate for a later iteration | Accepted |
| DEV-06 | State `UTLÅNT` | Contains `Å`. The verifier keeps `UTLÅNT` as the label but uses the ID `UTLNT` | A renamed state would show as Changed | Try the UTF-8 identifier `UTLÅNT` first (GCC 10+ accepts it); record how the verifier compares it | Open |
| DEV-07 | — (not in any diagram) | Tilt selection, LED ✓/✗, beeps and the serial protocol are not modelled | None: the code is outside `project/impl/` | Specified in `firmware/` and the protocol spec (milestone 3) | Accepted |
| DEV-08 | Association `Utlaan` → `Lab_Utstyr` (`0..*` / `1`) | The connector is not attached at both ends. **The verifier does not read this relation**; only 4 relations appear in `class-design.mmd` | **M1 result:** the attribute `Utlaan.utlaant_utstyr` is Missing, and the relation `Utlaan → Lab_Utstyr (utlaant_utstyr)` is Extra | Implemented as `Lab_Utstyr* utlaant_utstyr_`, as the arrow means. A diagram fix (attaching the arrow) is a candidate for a later iteration | Accepted |
| DEV-09 | State events `regUtlaan()` / `regRetur()` | They are loose text labels, not labels on the arrows. **The verifier reads both transitions with no event** and warns about it | Implemented events will show as Changed transitions | Implement the drawn events anyway (they are the source's meaning); accept the Changed entries | Open |
| DEV-10 | Initial state (start symbol → `LEDIG`) | The start arrow is not attached at both ends. **The verifier reads no initial state** | `State state_ = State::LEDIG;` may count as an Extra | Implement `LEDIG` as the start state (the source's meaning); record the verifier's result in milestone 2 | Open |
| DEV-11 | Sequence actor `Lab Ansatt`; return `Laan` | "Lab Ansatt" is a separate text box, so the verifier names the actor `Actor`. The dashed `Laan` return is read as `Objekt → Lab` | Return arrows are not compared; the actor name does not affect the call messages | None needed; recorded so the reading is known | Accepted |
| DEV-12 | Attributes `utstyrsliste`, `student_liste`, `utlaansliste`, `mine_laan`, `laanetaker`, `utlaant_utstyr` | Each is drawn **both** as an attribute and as a relation (without a role name). The mapping says a member that implements a relation is not also an attribute, and a relation is identified by its role name | **M1 result:** the 3 `Lab` relations without a role name are Missing, and their named versions (`utstyrsliste`, `student_liste`, `utlaansliste`) are Extra. The attribute `laanetaker` is Missing and the relation `Utlaan → Student (laanetaker)` is Extra | No C++ member can match a drawing that shows it twice. A diagram fix (role names on the lines, the attributes removed from the boxes) is a candidate for a later iteration | Accepted |
| DEV-13 | `Student.mine_laan` + `Student --> "0..*" Utlaan` | **Extractor limitation:** the mapping says pointer = association and vector = `*`, but `cpp_extract.py` keeps `std::vector<Utlaan*>` as an attribute | **M1 result:** the relation `Student → Utlaan` is Changed (dependency instead of association), and the attribute `mine_laan` is Extra | The C++ follows the mapping document, not the extractor's behaviour | Accepted |
| DEV-14 | Attributes of type `<Class>[0..*]` (`utstyrsliste`, `student_liste`, `utlaansliste`, `mine_laan`); `get_laan(): Utlaan[0..*]` | **Design-reader behaviour:** the attributes are not counted at all, and the type `Utlaan[0..*]` is not recognised as a list | **M1 result:** the attributes are neither Missing nor Identical. `Student.get_laan()` is Changed (return type `Utlaan[0..*]` vs `vector<Utlaan>`) | A diagram fix (`vector<Utlaan>` as the return type) is a candidate for a later iteration | Accepted |

The verifier's reading of each diagram is in `build/verify/diagrams/output/*-design.mmd`, which
`fom/measure_fom.py` regenerates on every run.
