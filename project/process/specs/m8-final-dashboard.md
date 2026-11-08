# M8 spec: final iteration, a cleaner dashboard that stores only successful actions

**The change (team, 2026-11-08):**
1. The *Lagring* and *Tilkobling* sections are smaller, to make room for the rest of the page.
2. *Valgt på stasjonen* and *Utstyrsstatus* are next to each other.
3. The code names in parentheses are removed from the page, to make it cleaner.
4. The CSV file only stores **successful** actions. A failed attempt, for example returning an item that cannot
   be returned, is not stored.

Then the final FoM and benchmark are measured, and the last three iterations (6, 7, 8) are shown in two charts.

Only `dashboard/` changes. The diagrams and `project/impl/` are not changed, so the verifier reports should be the
same as in iteration 7.

## Changes and how each maps to the code

| Change | Where | How |
|---|---|---|
| 1. Smaller storage and connection | `index.html`, `styles.css` | Both in one row (`.rad.kontroll`, two columns), with smaller padding, headings, status text and buttons. The storage hint is shorter |
| 2. Selection next to item status | `index.html`, `styles.css` | One row (`.rad.oversikt`): selection 1 part, item status 2 parts. Below 720 px wide, every row stacks |
| 3. No code names in parentheses | `index.html` | "Student (`studentnummer`)" → "Student", "Utstyr (`utstyr_navn`)" → "Utstyr", "Utstyrsstatus (`Lab_Utstyr`)" → "Utstyrsstatus". **Also** (same reason, not in parentheses): the column headers `utstyr_navn` and `studentnummer` → "Utstyr" and "Student" |
| 4. Store only successful actions | `csv_format.js`, `main.js` | `isLogged(event)`: a `LAAN_UT` or `RETUR` **with result OK**. Only such events are written to the file **and listed** on the page. A failed attempt only updates the selection and the item table; the station shows it with ✗ and a low beep. The text above the list says so |

**First version, corrected after the board test (team, 2026-11-08):** a failed attempt was first still listed on
the page, only not stored. In the board test the team saw the FEIL rows in the list and read them as tracked
(the saved file itself held only OK rows). Failed attempts are now not listed either.

Files written by iteration 7 can contain FEIL rows. They are left in the file, and not shown.

## Requirements

- REQ-STO-01 changed (only OK is stored), REQ-UI-01 changed (only OK is listed), REQ-UI-03 new (the layout).
  REQ-TST-01 updated with the test counts.

## Verification

- JS unit tests: what is stored and listed (`isLogged`), including a failed return and a failed loan, and an
  iteration-7 file with FEIL rows.
- Browser run of the real page with a simulated micro:bit and an in-memory file (`tests/browser/run.mjs`): the
  golden lines' 6 loans/returns give exactly 4 stored rows; all 6 listed; reload + open file shows the 4; layout
  measured at 1280 px and 390 px; screenshots kept.
- The verifier on unchanged `project/impl/`: expected the same reports as iteration 7.

## Measurement environment

WSL is no longer installed on the PC, so the FoM and the benchmark run on native Windows with MSYS2 (decision
D-18). Iteration 7's tagged code was re-measured there first and reproduced its logged numbers and reports exactly.
