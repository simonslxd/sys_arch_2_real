# Final conformance report: micro:bit lab-lending IoT prototype

Status after iteration 7 (2026-10-29). Repository: github.com/simonslxd/sys_arch_2_real, tags `iter-0` to `iter-7`.
Iteration 7 added one feature at the team's request: the dashboard shows "x av y ledig" for every item
([specs/m7-equipment-counts.md](specs/m7-equipment-counts.md)).

## 1. What was built

A lab-equipment lending station. The domain model comes from the team's UML diagrams (class, state and sequence)
in `project/diagrams/input/`, which were **not changed** in this iteration (team decision).

```
micro:bit V2 (C++, CODAL)                                    PC browser (Chrome/Edge)
 buttons A/B, tilt ─► InputHandler ─► StationController ─┐    serial_link ─► event_parser ─► dashboard_view
                       (polls 20 ms)   (selection)        │      (WebSerial)                   (Norwegian page)
                                         │                │                     └─► csv_format/csv_store
                                         ▼                │                          (CSV file on the PC:
                         domain model project/impl ◄──────┘                           loans and returns)
                         Lab, Lab_Utstyr, Student, Utlaan
 LEDs ✓/✗, beeps ◄── FeedbackOutput        event_writer ──── USB serial, 115200 baud ───►
```

- **One domain library, used in three places:** the verifier checks it, the PC unit tests and the benchmark run it,
  and the micro:bit runs it. So the code on the device is the code checked against the model.
- **The serial protocol is a model of its own:** `spec/serial-protocol.md`, with 13 golden lines. The firmware's
  C++ writer and the dashboard's JS parser are both tested against those lines.

## 2. Figure of Merit

FoM = (A + T + R) / 3, where A = mean model alignment (class, state, sequence report), T = test pass rate and
R = requirements verified. Every iteration's reports are archived in `fom/history/iter-N/`, and
[fom/PROGRESS.md](../../fom/PROGRESS.md) links them.

| Iteration | Milestone | A | T | R | **FoM** | Benchmark |
|--:|---|--:|--:|--:|--:|--:|
| 0 | baseline (no code) | 0.0 | 0.0 | 0.0 | **0.0** | – |
| 1 | domain classes | 20.7 | 100.0 (14 tests) | 10.5 | **43.8** | – |
| 2 | state machine + loan scenario | 30.7 | 100.0 (20) | 21.1 | **50.6** | – |
| 3 | micro:bit firmware | 30.7 | 100.0 (34) | 57.9 | **62.9** | – |
| 4 | dashboard, live | 30.7 | 100.0 (41) | 63.2 | **64.6** | – |
| 5 | CSV storage | 30.7 | 100.0 (50) | 89.5 | **73.4** | – |
| 6 | benchmark, SRP review, this report | 30.7 | 100.0 (50) | 100.0 | **76.9** | 87 ns |
| 7 | available counts on the dashboard | 29.8 | 100.0 (59) | 100.0 | **76.6** | 65 ns |

The FoM is deterministic: two runs on the same code gave byte-identical reports (checked in iterations 1 and 2),
and re-measuring the tagged code of iterations 0 and 1 reproduced their logged numbers. The benchmark (ns per loan
+ return, lab of 100 students and 100 items) varied between 65 and 116 ns on the same PC, so it is logged but kept
out of the FoM.

### Benchmark across versions

The benchmark program only exists since iteration 6, so `fom/benchmark_versions.py` builds it against each tagged
iteration's `project/impl/` and measures all versions in one session, alternating between them (5 rounds).
Numbers: [fom/benchmark-versions.csv](../../fom/benchmark-versions.csv).

![Time per loan + return by iteration](../../fom/benchmark-versions.svg)

- The only real change is **iter-1 → iter-2**. There, `laan_ut` started looking up the student (A-11, from the
  sequence diagram's `finnStudent`), and `Lab_Utstyr` got its state machine. On the 100 × 100 lab the median went
  from 54 to about 66–72 ns; on the 3 × 3 lab from 21 to about 23–29 ns.
- iter-3 to iter-6 run exactly the same domain code (checked with `git diff`), so the differences between them,
  and the single slow runs (up to 114 ns), are measurement noise. That's also why the 87 ns in the FoM log for
  iteration 6, measured in another session, differs from the 66 ns median here.

**What limits the FoM is A (30.7 %), and that comes from the diagrams, not the code** (section 4). Every drawn
element is implemented. Section 5 lists the draw.io changes that would remove most of the differences.

## 3. Requirements: 21 of 21 verified

Full table with the evidence for each: [specs/requirements.md](specs/requirements.md).

| Area | Requirements | How verified |
|---|---|---|
| Hardware | REQ-HW-01 to 05 (buttons, tilt, LEDs, speaker) | Unit tests of `InputDecoder` and `StationController`; 2 recorded hardware runs, each **13 of 13** lines reproduced by replaying the board's inputs through the PC-tested code (`tests/hardware/replay_log`); LEDs and beeps observed by the tester |
| Domain | REQ-DOM-01 to 03 | Verifier reports (section 4) + deviation log |
| Communication | REQ-COM-01, 02 | Golden-lines contract test (C++ and JS); hardware logs; browser test |
| Storage, UI | REQ-STO-01, 02, REQ-UI-01 | CSV tests; the browser test's file held exactly the 4 loans/returns given; reload showed the history |
| Tests, verification | REQ-TST-01, 02, REQ-VER-01, 02 | 40 C++ + 19 JS tests; benchmark; deterministic verifier; 8 logged iterations |
| Counts (iteration 7) | REQ-UI-02, REQ-COM-03 | Golden `STATUS` lines (C++ and JS); `STATUS` answered over COM3; the browser test's CSV file: 11 of 11 count changes follow the rules |
| Process | REQ-PRO-01, 02 | Tags on GitHub; [specs/srp-review.md](specs/srp-review.md) |

**Read with care:**
- REQ-DOM-01 to 03 are "verified with logged deviations": the code implements every drawn element, but the
  reports are below 100 % (section 4).
- REQ-HW-04, REQ-HW-05, REQ-COM-02, REQ-STO-02 and REQ-UI-01 rest on the tester's observation, without a recording.
- REQ-STO-01 and REQ-UI-01 were **changed by the team** during M5: only `LAAN_UT`/`RETUR` (OK and FEIL) are stored
  and listed, with event names only. The old wording is kept in the table.

## 4. Conformance with the UML models

| Diagram | Alignment | Identical | Differences |
|---|--:|--:|---|
| Class | 54.8 % | 23 of 42 | 2 changed, 5 missing, 12 extra |
| State (`Lab_Utstyr`) | 22.2 % | 2 of 9 | 2 missing, 5 extra |
| Sequence (loan) | 12.5 % | 2 of 16 | 8 missing, 6 extra |

Every difference is explained in [specs/deviations.md](specs/deviations.md) (DEV-01 to DEV-17, none open). By
cause:

| Cause | Deviations | Effect |
|---|---|---|
| **Labels or arrows drawn but not attached**, so the verifier doesn't read them | DEV-05 (`get_navn()`), DEV-08 (`Utlaan → Lab_Utstyr`), DEV-09 (state events), DEV-10 (start arrow), DEV-11 (actor name) | Drawn elements count as missing; their implementation counts as extra |
| **The same thing drawn twice:** a list both as an attribute and as a relation without a role name | DEV-12 | Each relation is missing once and extra once |
| **Sequence lifelines and messages that aren't classes or methods** in the class diagram | DEV-02 | Almost no sequence message can match |
| **Elements the mapping requires but the class box doesn't show** | DEV-03 (`state`, `handle`), DEV-15 (`get_state`), DEV-17 (`get_antall_tilgjengelig`, `get_antall_totalt`, iteration 7) | Extra in the class report |
| **Types the verifier doesn't understand** | DEV-14 (`Utlaan[0..*]`) | Changed |
| **Verifier limitation** (not fixable by drawing) | DEV-13 (`vector<Utlaan*>` read as an attribute) | 1 changed relation + 1 extra attribute |
| Naming and file names | DEV-01, DEV-04, DEV-06 | Resolved or handled (renamed copies, `UTLÅNT` as a UTF-8 identifier) |
| Getters missing from the model | DEV-16 | The station keeps student numbers itself (the counts have getters since iteration 7) |

## 5. Candidate diagram fixes for the next iteration

Each is a small change in draw.io. Doing them in a separate commit shows their effect in the FoM log.

**Class diagram**
1. Give the four relations role names (`utstyrsliste`, `student_liste`, `utlaansliste`, `mine_laan`) and remove the
   same names from the attribute lists. Draw `Utlaan → Student` with role `laanetaker`, and remove that attribute.
   Resolves DEV-12.
2. Attach the `Utlaan → Lab_Utstyr` arrow, give it role `utlaant_utstyr`, and remove the attribute. Resolves DEV-08.
3. Move `+ get_navn(): string` into the `Lab_Utstyr` box. Resolves DEV-05.
4. Add `- state: State`, `+ handle(event: Event): void`, `+ get_state(): State`,
   `+ get_antall_tilgjengelig(): int` and `+ get_antall_totalt(): int` to `Lab_Utstyr`. Resolves DEV-03, DEV-15 and
   DEV-17.
5. Write `get_laan()`'s return type as `vector<Utlaan>`, and `Utlån` as `Utlaan`. Resolves DEV-14 and DEV-04.
6. Optional: add `get_studentnummer()` to `Student`. Then the station no longer needs its own list of student
   numbers (DEV-16), but that needs a code change too.

**State diagram**

7. Attach `regUtlaan()` and `regRetur()` to their arrows, and attach the start arrow to `LEDIG`. Resolves DEV-09
   and DEV-10. All differences in the state report come from these two causes.

**Sequence diagram**

8. Use the class names as lifelines (`Lab`, `Lab_Utstyr`, `Student`), and draw the calls the code makes in this
   order: `laan_ut`, `er_tilgjengelig`, `reduser_antall_tilgjengelig`, `legg_til_laan`. Put "Lab Ansatt" in the
   actor's own label. Resolves DEV-02 and DEV-11.

**Files**

9. Optional: save the diagrams as `class.drawio`, `state-lab_utstyr.drawio` and `sequence-laan_ut.drawio`. Then the
   verifier can run on `project/` directly, without the renamed copies (DEV-01).

DEV-13 is not fixed by drawing: the verifier reads `std::vector<Utlaan*>` as an attribute. It needs either a
verifier change or a different C++ type for `mine_laan`, which is a decision for the team.

## 6. Open issues and limitations

1. **Two serial lines (seq 86, 87) are missing** from the first CSV test file, and the cause is unknown. The
   dashboard has warned about `seq` gaps since then, and no warning was reported in the second run.
2. **CODAL's event bus did not deliver button and tilt events** to our handlers on this board, while polling saw
   every press. The firmware therefore polls (D-11). The root cause inside CODAL was not identified.
3. **The hardware was last tested with the M3 firmware.** M6 changed `InputHandler` from an implicit to an explicit
   `int` → `bool` conversion (`!= 0`, same behaviour) and was not re-run on the board.
4. **A correction:** the M3 report claimed 0 firmware warnings; there were 2, as explained in
   [specs/m3-firmware.md](specs/m3-firmware.md). Both are now fixed.
5. **The benchmark depends on the machine** (65–116 ns on one PC).
6. **Some evidence is the tester's observation** and was not recorded (section 3).

## 7. How the AI-generated code was kept honest

1. **Structure:** libclang reads the C++ headers, and the verifier compares them with the drawings. No AI judgement
   is involved.
2. **Behaviour:** the state machine is compared as a transition table, and the loan scenario is recorded by running
   it.
3. **Interface:** the same 13 golden protocol lines are checked by the C++ test and the JS test.
4. **Hardware:** recorded serial logs are replayed through the PC-tested station code (13 of 13 lines, twice). The
   replay was itself checked by feeding it a wrong line, which it caught.
5. **Traceability:** every requirement points to its evidence, and every deviation, assumption (A-01 to A-15) and
   decision (D-01 to D-17) has an ID. Every iteration is a tagged commit, with its reports archived.
6. **Human gates:** the team approved each milestone before it was committed and pushed.

## 8. Reproduce it (in WSL, from the repository root)

```bash
.venv/bin/python fom/measure_fom.py            # verifier + C++ and JS tests + requirements + benchmark
bash firmware/build.sh                         # build/MICROBIT.hex, then copy it onto the MICROBIT drive
python3 dashboard/serve.py                     # then open http://localhost:8000 in Chrome or Edge
git checkout iter-N                            # any earlier iteration
```
