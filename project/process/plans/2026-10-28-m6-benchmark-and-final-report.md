# M6: Benchmark, SRP review and final conformance report

Goal: the last two requirements (REQ-TST-02, REQ-PRO-02) and one report that pulls the whole project together.

## Steps

- [x] `benchmarks/lending_benchmark.cpp` + CMake (Release): 10 000 loan + return pairs, median of 7, labs of 3×3
      and 100×100 (D-17).
- [x] `fom/measure_fom.py` builds and runs it and logs `bench_ns`; `PROGRESS.md` shows it next to the FoM.
- [x] SRP review of every module ([srp-review.md](../specs/srp-review.md)); 2 overstated header comments corrected.
- [x] Found while rebuilding: 2 `-Wnarrowing` warnings in `InputHandler` since M3, missed because the M3 warning
      count read an old log. Fixed (`!= 0`); M3 record corrected.
- [x] `requirements.md`: REQ-TST-02, REQ-PRO-02 verified; D-17.
- [x] Log FoM iteration 6.
- [x] Final conformance report ([final-conformance-report.md](../final-conformance-report.md)).
- [x] The team reviews, commits and tags `iter-6`.
