# M1: Domain classes

Goal: the four classes of the class diagram in `project/impl/`, plain C++20 with no hardware calls, with unit
tests and FoM measurement #1. Spec: [m1-domain-classes.md](../specs/m1-domain-classes.md).

## Steps

- [x] Read the mapping rules and the extractor (`tools/umlverify/class_diagram/cpp_extract.py`, `elements.py`)
      to predict how each member is compared.
- [x] Write the spec: each drawn element → its C++ construct, plus the expected verifier result.
- [x] `project/impl/`: `CMakeLists.txt`, 4 headers in `include/labutlaan/`, 4 sources + `demo.cpp` in `src/`.
- [x] Build in WSL with `-Wall -Wextra`: no warnings. The demo shows loan → second loan refused → return.
- [x] `tests/cpp/`: a minimal runner (`check.hpp`) and 14 tests over the 4 classes; 14 of 14 pass.
- [x] `fom/measure_fom.py`: builds and runs the C++ tests for T; sets `CPATH` so libclang can parse (D-07).
- [x] Run the verifier: class report 62.2 %, every difference explained (DEV-05, DEV-08, DEV-12, DEV-13, DEV-14).
- [x] Determinism check: two runs gave 8 byte-identical output files (REQ-VER-01).
- [x] Update `requirements.md` (REQ-DOM-01 and REQ-VER-01 verified; A-08 to A-10; D-07, D-08) and
      `deviations.md`.
- [x] Log FoM iteration 1.
- [x] The team reviews, commits and tags `iter-1`.

## How to reproduce (in WSL, from the repository root)

```bash
cmake -S tests/cpp -B build/tests && cmake --build build/tests && ./build/tests/domain_tests
.venv/bin/python fom/measure_fom.py --log 1
```
