# M1 spec: domain classes

Source: `project/diagrams/input/lab_utlaan_klassediagram(3).drawio`, as read by the verifier
(`build/verify/diagrams/output/class-design.mmd`). Mapping rules:
[UML-CPP-MAPPING.md](../../../tools/umlverify/docs/UML-CPP-MAPPING.md).

Namespace `labutlaan`. One header per class in `project/impl/include/labutlaan/`.

## Classes, members and their C++ construct

### `Lab`: `lab.hpp`

| Design | C++ | Mapping rule |
|---|---|---|
| `- utstyrsliste: Lab_Utstyr[0..*]` + `Lab o-- "0..*" Lab_Utstyr` | `std::vector<std::shared_ptr<Lab_Utstyr>> utstyrsliste_` | hollow diamond + `*` = vector of `shared_ptr` |
| `- student_liste: Student[0..*]` + `Lab o-- "0..*" Student` | `std::vector<std::shared_ptr<Student>> student_liste_` | same |
| `- utlaansliste: Utlaan[0..*]` + `Lab *-- "0..*" Utlaan` | `std::vector<std::unique_ptr<Utlaan>> utlaansliste_` | filled diamond + `*` = vector of `unique_ptr` |
| `+ legg_til_utstyr(utstyr: Lab_Utstyr): void` | `void legg_til_utstyr(std::shared_ptr<Lab_Utstyr> utstyr)` | the lab becomes one of the item's owners |
| `+ registrer_student(student: Student): void` | `void registrer_student(std::shared_ptr<Student> student)` | same |
| `+ laan_ut(student: Student, utstyr: Lab_Utstyr): Utlaan` | `Utlaan* laan_ut(Student& student, Lab_Utstyr& utstyr)` | the lab owns the loan, so the caller gets a pointer to it; `nullptr` when it can't be lent |
| `+ registrer_retur(utlaan: Utlaan): void` | `void registrer_retur(Utlaan& utlaan)` | |
| `+ finn_tilgjengelig_utstyr(utstyr: Lab_Utstyr): Bool` | `bool finn_tilgjengelig_utstyr(const Lab_Utstyr& utstyr) const` | `Bool` and `bool` compare equal |

### `Lab_Utstyr`: `lab_utstyr.hpp`

| Design | C++ |
|---|---|
| `- utstyr_navn: string` | `std::string utstyr_navn_` |
| `- antall_totalt: int` | `int antall_totalt_` |
| `- antall_tilgjengelig: int` | `int antall_tilgjengelig_` |
| `+ er_tilgjengelig(): bool` | `bool er_tilgjengelig() const` |
| `+ oek_antall_tilgjengelig(): void` | `void oek_antall_tilgjengelig()` |
| `+ reduser_antall_tilgjengelig(): void` | `void reduser_antall_tilgjengelig()` |
| `+ get_navn(): string` (drawn under the box, not read by the verifier: DEV-05) | `std::string get_navn() const` |

### `Student`: `student.hpp`

| Design | C++ | Note |
|---|---|---|
| `- studentnummer: string` | `std::string studentnummer_` | |
| `- navn: string` | `std::string navn_` | |
| `- mine_laan: Utlån[0..*]` + `Student --> "0..*" Utlaan` | `std::vector<Utlaan*> mine_laan_` | open arrow + `*` = vector of raw pointers; the student refers to loans that `Lab` owns |
| `+ legg_til_laan(Utlaan): void` | `void legg_til_laan(Utlaan& utlaan)` | parameter name added (DEV-04) |
| `+ fjern_laan(Utlaan): void` | `void fjern_laan(Utlaan& utlaan)` | parameter name added (DEV-04) |
| `+ get_laan(): Utlaan[0..*]` | `const std::vector<Utlaan*>& get_laan() const` | |

### `Utlaan`: `utlaan.hpp`

| Design | C++ | Note |
|---|---|---|
| `- laanetaker: Student` | `Student* laanetaker_` | the loan refers to its borrower, it doesn't own them |
| `- utlaant_utstyr: Lab_Utstyr` + unattached arrow to `Lab_Utstyr` (DEV-08) | `Lab_Utstyr* utlaant_utstyr_` | same |
| `- antall: int` | `int antall_` | |
| `+ get_laanetaker(): Student` | `Student& get_laanetaker() const` | |
| `+ get_utstyr(): Lab_Utstyr` | `Lab_Utstyr& get_utstyr() const` | |
| `+ get_antall(): int` | `int get_antall() const` | |

Constructors are allowed and not compared: `Lab_Utstyr(navn, antall_totalt)`, `Student(studentnummer, navn)`
and `Utlaan(laanetaker, utlaant_utstyr, antall)`.

## Behaviour (method bodies in `src/`)

The diagram gives names and types only. The behaviour below is the smallest one that fits those names. Each
rule not stated by a model is an assumption (A-xx in [requirements.md](requirements.md)).

- `Lab_Utstyr`: starts with `antall_tilgjengelig = antall_totalt`. `er_tilgjengelig()` is true while
  `antall_tilgjengelig > 0`. `reduser` never goes below 0, and `oek` never goes above `antall_totalt` (A-09).
- `Lab::finn_tilgjengelig_utstyr(utstyr)`: true when `utstyr` is in this lab's `utstyrsliste` **and**
  `utstyr.er_tilgjengelig()` (A-08).
- `Lab::laan_ut(student, utstyr)`: if `finn_tilgjengelig_utstyr(utstyr)` is false, it returns `nullptr` and
  changes nothing. Otherwise it reduces the item's count, creates an `Utlaan` with `antall = 1` (A-04), adds it to
  `utlaansliste` and to the student's `mine_laan`, and returns it.
- `Lab::registrer_retur(utlaan)`: if the loan belongs to this lab, it increases the item's count, removes the
  loan from the student and from `utlaansliste` (destroying it). Otherwise it does nothing.

The failure cases (`nullptr`, ignoring a foreign loan) are assumption A-10.

## Verifier result (M1)

**Class alignment: 62.2 %**: 23 identical, 2 changed, 5 missing, 7 extra. All 4 classes, the 6 attributes with
simple types and 13 of the 15 methods are identical. The predicted value was about 55 %. The difference is that
the design reader doesn't count the `[0..*]` attributes at all (DEV-14), so they aren't Missing.

| Difference | Cause | Deviation |
|---|---|---|
| Missing: relations `Lab → Lab_Utstyr`, `Lab → Student`, `Lab → Utlaan` (no role name). Extra: the same relations named `utstyrsliste`, `student_liste`, `utlaansliste` | A relation is identified by its role name, and the drawn lines have none | DEV-12 |
| Missing: attribute `Utlaan.laanetaker`. Extra: relation `Utlaan → Student (laanetaker)` | A member typed with a class is a relation, never an attribute | DEV-12 |
| Missing: attribute `Utlaan.utlaant_utstyr`. Extra: relation `Utlaan → Lab_Utstyr (utlaant_utstyr)` | Same, and the drawn arrow is not attached | DEV-08 |
| Extra: method `Lab_Utstyr.get_navn()` | Drawn outside the class box, so not read | DEV-05 |
| Changed: relation `Student → Utlaan` (dependency instead of association). Extra: attribute `Student.mine_laan` | The extractor keeps `std::vector<Utlaan*>` as an attribute | DEV-13 |
| Changed: `Student.get_laan()` return type | `Utlaan[0..*]` isn't recognised as a list | DEV-14 |

Every element drawn in the class diagram is implemented. No remaining difference can be removed by changing the
C++ without departing from the mapping document or from the drawing. The candidate diagram fixes for a later
iteration are listed in DEV-05, DEV-08, DEV-12 and DEV-14.
