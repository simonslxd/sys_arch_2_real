# M5: CSV storage

Goal: every loan and return received is stored in a CSV file on the PC, and an earlier file's history is shown
again after a reload. Spec: [m5-csv-storage.md](../specs/m5-csv-storage.md).

## Steps

- [x] `dashboard/js/csv_format.js` (format, reading, logging rule) and `csv_store.js` (File System Access).
- [x] Page: *Ny CSV-fil* / *Åpne CSV-fil*, storage status, *Mottatt* and *Kilde* columns; connecting needs a file.
- [x] Browser run 1: valid file (24 rows, 0 rejected); seq 86–87 missing, unexplained.
- [x] Team decision: store and list only `LAAN_UT`/`RETUR`; show event names without labels.
- [x] `seq` gap warning (D-16); tests for the logging rule and gaps; 16 of 16 JS tests pass.
- [x] Stale JS modules in Chrome after the update → `dashboard/serve.py` without caching (D-14).
- [x] Browser run 2: 4 loans/returns stored exactly; reload + reopen worked.
- [x] `requirements.md`: REQ-STO-01, REQ-STO-02, REQ-UI-01 (reworded), REQ-TST-01, REQ-PRO-01 verified;
      A-14, A-15; D-14 updated, D-15, D-16. README, checklist, spec.
- [x] Log FoM iteration 5.
- [x] The team reviews, commits and tags `iter-5`.
