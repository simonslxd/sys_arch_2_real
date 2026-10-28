# M0: Setup and baseline

Goal: a reproducible toolchain, a git history on GitHub, and a measured FoM baseline before any IoT code
exists. The architecture and the milestone list were approved on 2026-10-28.

## Steps

- [x] Check the toolchain. Native Windows has none of it, and the verifier can't check sequence diagrams there
      (it expects `scenario_<name>` without `.exe`, plus GCC/Clang and `nm`). Decision D-01: use WSL Debian.
- [x] Install the toolchain in WSL Debian 13 (done by the team: needs sudo). Python 3.13.5, CMake 3.31.6,
      g++ 14.2.0, Node 20.19.2, arm-none-eabi-gcc 14.2.1.
- [x] Create `.venv/` in WSL with libclang (`requirements.txt`); `import clang.cindex` works.
- [x] `git init -b main`; set the repository-local identity to `simonslxd`; the team published the baseline
      commit to `github.com/simonslxd/sys_arch_2_real` from VS Code; tag it `iter-0`.
- [x] Stop tracking `.claude/settings.local.json` (personal permissions; it changes on every approval).
- [x] Write `project/process/specs/requirements.md`: requirements, assumptions and decisions, kept apart.
- [x] Write `project/process/specs/deviations.md`: DEV-01 to DEV-12, including how the verifier reads each
      diagram.
- [x] Write `fom/measure_fom.py`.
- [x] Measure and log the baseline (iteration 0) in `fom/fom-log.csv`.
- [ ] The team reviews and commits the M0 files, then pushes the commit and the `iter-0` tag.

## How to reproduce (in WSL, from the repository root)

```bash
.venv/bin/python fom/measure_fom.py           # measure only
.venv/bin/python fom/measure_fom.py --log N   # measure and log iteration N
```

## Result

A = 0.0 %, T = 0.0 %, R = 0.0 %, FoM = 0.0 %. `project/impl/` is empty, so the verifier converts all three
designs but compares nothing.

The logged commit is `iter-0-dirty`: this run is the `iter-0` implementation plus the uncommitted M0 tooling.
`project/impl/` is identical to `iter-0`.
