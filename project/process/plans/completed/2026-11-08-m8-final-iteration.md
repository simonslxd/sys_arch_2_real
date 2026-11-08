# M8: Final iteration: dashboard layout, store only successful actions, final FoM and benchmark

Goal: the four dashboard changes, then the final measurements and two charts of iterations 6–8. Spec:
[m8-final-dashboard.md](../specs/m8-final-dashboard.md).

## Steps

- [x] Toolchain: WSL is gone; install CMake, Ninja, libclang, Node with MSYS2 (approved by the team). Make the
      FoM script work on Windows (`fom/windows/`, D-18).
- [x] Validate the Windows toolchain: re-measure the tagged iteration 7. Result: A 29.8, T 100 (59/59), R 100,
      FoM 76.6; reports byte-identical to the WSL archive.
- [x] Dashboard: storage rule (`isLogged` = OK only, `isLoanOrReturn` for the list), layout, labels.
- [x] JS tests for listing and storing; 21/21 pass.
- [x] Browser run with a simulated micro:bit (`tests/browser/`): 8/8 checks pass; results kept.
- [x] `requirements.md`: REQ-STO-01, REQ-UI-01 changed, REQ-UI-03 added, D-18 added.
- [x] Log FoM iteration 8.
- [x] Benchmark iterations 6, 7 and 8 in one session (`benchmark_versions.py`).
- [x] Charts of FoM and benchmark over iterations 6–8 (`fom/final_charts.py`).
- [x] Update the READMEs and the final conformance report.
- [x] Board test with the micro:bit (twice; the second after failed attempts were also taken off the list): "works".
- [x] The team approves; commit and tag `iter-8`.
