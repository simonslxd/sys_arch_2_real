#!/usr/bin/env python3
"""Measure the Figure of Merit (FoM) of the repository as it is now.

    .venv/bin/python fom/measure_fom.py                  measure and print
    .venv/bin/python fom/measure_fom.py --log 3          also append the row for iteration 3
                                                         to fom/fom-log.csv

FoM = (A + T + R) / 3, every part in percent:

  A  model alignment       mean "Alignment" of the class, state and sequence reports
  T  test pass rate        passed unit tests / all unit tests
  R  requirement coverage  requirements marked Verified / all requirements

The same commit always gives the same A, T and R. Run it in WSL (decision D-01).

The input diagrams don't have the names the verifier expects (deviation DEV-01), and they
must not be edited. So the script copies them byte for byte, under the expected names, into
build/verify/ together with project/impl/, and runs tools/verify.py on that copy. A report
that is missing, or that has no alignment because the implementation is not there yet,
counts as 0 %.
"""
import argparse
import csv
import os
import re
import shutil
import subprocess
import sys
from datetime import date
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
PROJECT = REPO / "project"
VERIFY_DIR = REPO / "build" / "verify"
TEST_BUILD_DIR = REPO / "build" / "tests"
FOM_LOG = REPO / "fom" / "fom-log.csv"
REQUIREMENTS = PROJECT / "process" / "specs" / "requirements.md"

# input diagram (as drawn)              -> name the verifier expects
DIAGRAM_COPIES = {
    "lab_utlaan_klassediagram(3).drawio": "class.drawio",
    "TilstandDiagram.drawio": "state-lab_utstyr.drawio",
    "Sekvens.drawio": "sequence-laan_ut.drawio",
}
# one report per diagram above, in the same order
REPORTS = ["class-report.md", "state-lab_utstyr-report.md", "sequence-laan_ut-report.md"]

LOG_COLUMNS = ["iteration", "date", "commit", "A", "T", "R", "FoM", "bench_ns"]


def prepare_verify_project():
    """Rebuild build/verify/ from scratch: renamed diagram copies plus a copy of project/impl/."""
    if VERIFY_DIR.exists():
        shutil.rmtree(VERIFY_DIR)
    diagram_dir = VERIFY_DIR / "diagrams" / "input"
    diagram_dir.mkdir(parents=True)
    for drawn_name, verifier_name in DIAGRAM_COPIES.items():
        shutil.copyfile(PROJECT / "diagrams" / "input" / drawn_name, diagram_dir / verifier_name)
    shutil.copytree(PROJECT / "impl", VERIFY_DIR / "impl")


def compiler_builtin_headers():
    """The folder with the compiler's own headers (stddef.h, ...), e.g.
    /usr/lib/gcc/x86_64-linux-gnu/14/include."""
    run = subprocess.run(["c++", "-print-file-name=include"], capture_output=True, text=True)
    return run.stdout.strip()


def run_verifier():
    """Run tools/verify.py on build/verify/ and show its output.

    The pip libclang wheel lacks clang's built-in headers, so on Linux it stops at
    "'stddef.h' file not found". CPATH lends it GCC's copies (decision D-07).
    """
    env = dict(os.environ, CPATH=compiler_builtin_headers())
    subprocess.run([sys.executable, str(REPO / "tools" / "verify.py"), str(VERIFY_DIR)], env=env)


def report_alignment(report_name):
    """The alignment percentage in one report, or 0.0 when it has none."""
    report = VERIFY_DIR / "reports" / report_name
    if not report.is_file():
        return 0.0
    match = re.search(r"\*\*Alignment: ([\d.]+) %\*\*", report.read_text(encoding="utf-8"))
    return float(match.group(1)) if match else 0.0


def model_alignment():
    """A: the mean alignment over all reports, in percent."""
    alignments = [report_alignment(name) for name in REPORTS]
    for name, value in zip(REPORTS, alignments):
        print(f"  {name}: {value:.1f} %")
    return sum(alignments) / len(alignments)


def count_tests():
    """(passed, total) over all unit tests: the C++ tests in tests/cpp/, built from scratch.

    Tests that don't build, or that crash before their summary line, count as 0 of 0.
    """
    if TEST_BUILD_DIR.exists():
        shutil.rmtree(TEST_BUILD_DIR)
    for cmd in (["cmake", "-S", str(REPO / "tests" / "cpp"), "-B", str(TEST_BUILD_DIR)],
                ["cmake", "--build", str(TEST_BUILD_DIR)]):
        if subprocess.run(cmd, capture_output=True, text=True).returncode != 0:
            print("  the C++ tests do not build")
            return 0, 0
    run = subprocess.run([str(TEST_BUILD_DIR / "domain_tests")], capture_output=True, text=True)
    match = re.search(r"RESULT: (\d+) of (\d+) tests passed", run.stdout)
    if not match:
        print("  the C++ tests stopped before their summary line")
        return 0, 0
    return int(match.group(1)), int(match.group(2))


def test_pass_rate():
    """T: passed tests / all tests, in percent; 0 when there are no tests."""
    passed, total = count_tests()
    print(f"  tests: {passed} of {total} passed")
    return 100.0 * passed / total if total else 0.0


def requirement_coverage():
    """R: rows marked Verified in requirements.md / all requirement rows, in percent."""
    rows = [line for line in REQUIREMENTS.read_text(encoding="utf-8").splitlines()
            if line.startswith("| REQ-")]
    # columns: | ID | Requirement | Verified by | Status | Evidence |
    verified = [row for row in rows if row.split("|")[4].strip() == "Verified"]
    print(f"  requirements: {len(verified)} of {len(rows)} verified")
    return 100.0 * len(verified) / len(rows) if rows else 0.0


def current_commit():
    """The commit being measured, e.g. 'iter-1' or 'iter-1-2-g1a2b3c4-dirty'."""
    run = subprocess.run(["git", "describe", "--tags", "--always", "--dirty"],
                         cwd=REPO, capture_output=True, text=True)
    return run.stdout.strip() or "unknown"


def append_to_log(row):
    """Append one row to fom/fom-log.csv, writing the header first if the file is new."""
    is_new = not FOM_LOG.is_file()
    with FOM_LOG.open("a", newline="", encoding="utf-8") as log:
        writer = csv.DictWriter(log, fieldnames=LOG_COLUMNS)
        if is_new:
            writer.writeheader()
        writer.writerow(row)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--log", type=int, metavar="ITERATION",
                        help="append the result to fom/fom-log.csv as this iteration")
    args = parser.parse_args()

    prepare_verify_project()
    run_verifier()

    print("\n━━ Figure of Merit " + "━" * 47)
    a = model_alignment()
    t = test_pass_rate()
    r = requirement_coverage()
    fom = (a + t + r) / 3
    print(f"\n  A = {a:.1f} %   T = {t:.1f} %   R = {r:.1f} %   FoM = {fom:.1f} %")

    if args.log is not None:
        row = {"iteration": args.log, "date": date.today().isoformat(), "commit": current_commit(),
               "A": f"{a:.1f}", "T": f"{t:.1f}", "R": f"{r:.1f}", "FoM": f"{fom:.1f}",
               "bench_ns": "-"}
        append_to_log(row)
        print(f"  logged to {FOM_LOG.relative_to(REPO)}")


if __name__ == "__main__":
    main()
