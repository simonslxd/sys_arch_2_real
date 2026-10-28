# M2 spec: state machine of `Lab_Utstyr` and the loan scenario

Sources: `project/diagrams/input/TilstandDiagram.drawio` (verified as `state-lab_utstyr.drawio`) and
`project/diagrams/input/Sekvens.drawio` (verified as `sequence-laan_ut.drawio`). Mapping rules:
[UML-CPP-MAPPING.md](../../../tools/umlverify/docs/UML-CPP-MAPPING.md), sections *State machines* and
*Sequence diagrams*. Pattern copied from `examples/3-state-simple` and `examples/5-sequence-simple`.

## State machine

### As drawn, and as the verifier reads it

| Drawn | Read by the verifier |
|---|---|
| states `LEDIG`, `UTLÅNT` | the same 2 states |
| start symbol → `LEDIG` | **nothing**: the arrow is not attached (DEV-10) |
| `LEDIG → UTLÅNT`, labelled `regUtlaan()` | a transition **without an event**: the label is loose text (DEV-09) |
| `UTLÅNT → LEDIG`, labelled `regRetur()` | the same |

### C++ (inside `Lab_Utstyr`)

| Drawn | C++ |
|---|---|
| the states | `enum class State { LEDIG, UTLÅNT };`, using the UTF-8 identifier `UTLÅNT` (DEV-06) |
| the events | `enum class Event { regUtlaan, regRetur };`, as drawn, without the parentheses |
| start → `LEDIG` | `State state_ = State::LEDIG;` |
| the 2 arrows | `static constexpr Transition transitions[]`, 2 rows, no guards and no actions |
| firing an event | `void handle(Event event)` in `src/`, which walks the table |
| — | `State get_state() const`, so tests, the firmware (M3) and the dashboard can see the state (DEV-15) |

**When the events fire (A-05):** the drawn machine is binary, but an item has several units. The item fires
`regUtlaan` when its **last** unit is lent (`reduser_antall_tilgjengelig` reaches 0). It fires `regRetur` when
a unit comes back to an item that had **none** (`oek_antall_tilgjengelig` leaves 0). The state is therefore
always `LEDIG` exactly while `antall_tilgjengelig > 0`. The table needs no guards for this: it stays as drawn.
An item created with 0 units fires `regUtlaan` in its constructor, so that rule holds from the start.

### Expected state report

| Element | Expected |
|---|---|
| states `LEDIG`, `UTLÅNT` | Identical |
| events `regUtlaan`, `regRetur` | Extra (the design has no events: DEV-09) |
| transitions `LEDIG on regUtlaan`, `UTLÅNT on regRetur` | Extra; the drawn `LEDIG on (no event)` and `UTLÅNT on (no event)` are Missing (DEV-09) |
| initial transition `[*] --> LEDIG` | Extra (the design has none: DEV-10) |

Expected alignment: 2 / (2 + 2 missing + 5 extra) ≈ **22 %**. That is the most the drawing allows, short of
leaving out the events and the initial state that are visibly drawn.

### Effect on the class report

The class box of `Lab_Utstyr` doesn't show `- state: State`, `+ handle(event: Event): void` or
`get_state()`, so they are 3 new Extras (DEV-03, DEV-15). `State`, `Event`, `Transition` and `transitions` are
nested and static, so the class extractor doesn't read them. Expected: class alignment from 62.2 % to about
57 %.

## Loan scenario

### As drawn (the verifier's reading)

```
Actor->>Lab: Laan(Objekt, Student)
Lab->>UtlaansObjekter: finnObjekt(Objekt)
Lab->>Objekt: erTilgjengelig()
Lab->>Studenter: finnStudent(Student)
Lab->>Objekt: settStudentPaaLaan()
```

### C++

`project/impl/scenarios/laan_ut.cpp`. `main()` creates a `Lab`, one `Lab_Utstyr` and one `Student` and registers
them (set-up, not part of the diagram). `scenario()` makes the actor's one call:
`lab.laan_ut(student, utstyr)`.

Each drawn message is mapped to the class diagram's nearest element (DEV-02):

| Drawn message | Implemented by | Recorded as a message? |
|---|---|---|
| `Laan(Objekt, Student)` | `Lab::laan_ut(student, utstyr)` | yes: `Actor → Lab : laan_ut` |
| `finnObjekt(Objekt)` on `UtlaansObjekter` | `Lab` searches its `utstyrsliste` in `finn_tilgjengelig_utstyr` | no: a call of `Lab` to itself |
| `erTilgjengelig()` on `Objekt` | `Lab_Utstyr::er_tilgjengelig()` | yes: `Lab → Lab_Utstyr : er_tilgjengelig` |
| `finnStudent(Student)` on `Studenter` | `Lab` searches its `student_liste` inside `laan_ut` (**new in M2**, A-11) | no: no other class is called |
| `settStudentPaaLaan()` on `Objekt` | `Lab_Utstyr::reduser_antall_tilgjengelig()`, then `Student::legg_til_laan(utlaan)` | yes: 2 messages, `Lab → Lab_Utstyr` and `Lab → Student` |

The recorded order is `laan_ut`, `er_tilgjengelig`, `reduser_antall_tilgjengelig`, `legg_til_laan`. That
follows the drawn order: find the item, check it, find the student, put the loan on the student.

### Expected sequence report

The drawn lifelines `UtlaansObjekter`, `Objekt` and `Studenter` and the drawn method names are not in the class
diagram, so no message can match by name: 5 Missing and 4 Extra messages. Lifelines: `Actor` and `Lab`
Identical; `UtlaansObjekter`, `Objekt`, `Studenter` Missing; `Lab_Utstyr`, `Student` Extra. Expected
alignment: 2 / 16 ≈ **12.5 %**. That is the most the drawing allows without classes that the class diagram
doesn't have.

## Verifier result (M2)

Every prediction above held: **state 22.2 %**, **sequence 12.5 %**, **class 57.5 %** (down from 62.2 %: the
Extras `state`, `handle()` and `get_state()`). The recorded scenario is
`Actor → Lab : laan_ut`, `Lab → Lab_Utstyr : er_tilgjengelig`, `Lab → Lab_Utstyr : reduser_antall_tilgjengelig`,
`Lab → Student : legg_til_laan`. The self-calls `Lab → Lab : finn_tilgjengelig_utstyr` and
`Lab_Utstyr → Lab_Utstyr : handle` are listed as *Not counted*.

## Change to `Lab::laan_ut` (A-11)

The sequence diagram has `Lab` find the student (`finnStudent`) before the loan is made. `laan_ut` now returns
`nullptr` when the student is not in the lab's `student_liste`, just as it already does for an item that isn't
in `utstyrsliste` (A-08). This rule comes from the sequence diagram; the class diagram alone doesn't state it.
