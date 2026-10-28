#!/usr/bin/env python3
"""Measure the Figure of Merit (FoM) of the repository as it is now.

    .venv/bin/python fom/measure_fom.py                  measure and print
    .venv/bin/python fom/measure_fom.py --log 3          also log it as iteration 3

FoM = (A + T + R) / 3, every part in percent:

  A  model alignment       mean "Alignment" of the class, state and sequence reports
  T  test pass rate        passed unit tests / all unit tests (C++ and JS)
  R  requirement coverage  requirements marked Verified / all requirements

The same commit always gives the same A, T and R. Run it in WSL (decision D-01).

The input diagrams don't have the names the verifier expects (deviation DEV-01), and they
must not be edited. So the script copies them byte for byte, under the expected names, into
build/verify/ together with project/impl/, and runs tools/verify.py on that copy. A report
that is missing, or that has no alignment because the implementation is not there yet,
counts as 0 %.

Logging iteration N (--log N) keeps everything needed to follow the FoM over time:
  fom/fom-log.csv          one row per iteration; logging N again replaces row N
  fom/history/iter-N/      that run's reports, the diagrams they link to, the test output
                           and the FoM summary
  fom/PROGRESS.md          the log as a table, with links into fom/history/
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
TEST_OUTPUT = TEST_BUILD_DIR / "output.txt"
FOM_LOG = REPO / "fom" / "fom-log.csv"
HISTORY = REPO / "fom" / "history"
PROGRESS = REPO / "fom" / "PROGRESS.md"
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


def model_alignment(summary):
    """A: the mean alignment over all reports, in percent."""
    alignments = [report_alignment(name) for name in REPORTS]
    for name, value in zip(REPORTS, alignments):
        summary.append(f"  {name}: {value:.1f} %")
    return sum(alignments) / len(alignments)


def cpp_tests():
    """(passed, total, output) of the C++ tests in tests/cpp/ (domain model and
    firmware/station), built from scratch. Tests that don't build, or that crash before
    their summary line, count as 0 of 0."""
    if TEST_BUILD_DIR.exists():
        shutil.rmtree(TEST_BUILD_DIR)
    TEST_BUILD_DIR.mkdir(parents=True)
    for cmd in (["cmake", "-S", str(REPO / "tests" / "cpp"), "-B", str(TEST_BUILD_DIR)],
                ["cmake", "--build", str(TEST_BUILD_DIR)]):
        build = subprocess.run(cmd, capture_output=True, text=True)
        if build.returncode != 0:
            return 0, 0, "The C++ tests do not build:\n\n" + build.stdout + build.stderr
    run = subprocess.run([str(TEST_BUILD_DIR / "unit_tests")], capture_output=True, text=True)
    match = re.search(r"RESULT: (\d+) of (\d+) tests passed", run.stdout)
    if not match:
        return 0, 0, run.stdout + run.stderr
    return int(match.group(1)), int(match.group(2)), run.stdout + run.stderr


def js_tests():
    """(passed, total, output) of the JS tests in tests/js/, run with Node's built-in test
    runner. No tests, or no Node, count as 0 of 0."""
    if not (REPO / "tests" / "js").is_dir():
        return 0, 0, "No JS tests yet.\n"
    try:
        run = subprocess.run(["node", "--test", "tests/js/"], cwd=REPO, capture_output=True, text=True)
    except FileNotFoundError:
        return 0, 0, "Node is not installed.\n"
    passed = re.search(r"^# pass (\d+)$", run.stdout, re.M)
    total = re.search(r"^# tests (\d+)$", run.stdout, re.M)
    if not (passed and total):
        return 0, 0, run.stdout + run.stderr
    return int(passed.group(1)), int(total.group(1)), run.stdout + run.stderr


def test_pass_rate(summary):
    """T: passed tests / all tests (C++ and JS), in percent; 0 when there are no tests.
    The output of both goes to build/tests/output.txt."""
    cpp_passed, cpp_total, cpp_output = cpp_tests()
    js_passed, js_total, js_output = js_tests()
    TEST_OUTPUT.write_text("━━ C++ tests (tests/cpp)\n" + cpp_output +
                           "\n━━ JS tests (tests/js)\n" + js_output, encoding="utf-8")
    passed, total = cpp_passed + js_passed, cpp_total + js_total
    summary.append(f"  tests: {passed} of {total} passed "
                   f"(C++ {cpp_passed}/{cpp_total}, JS {js_passed}/{js_total})")
    return 100.0 * passed / total if total else 0.0


def requirement_coverage(summary):
    """R: rows marked Verified in requirements.md / all requirement rows, in percent."""
    text = REQUIREMENTS.read_text(encoding="utf-8") if REQUIREMENTS.is_file() else ""
    rows = [line for line in text.splitlines() if line.startswith("| REQ-")]
    # columns: | ID | Requirement | Verified by | Status | Evidence |
    verified = [row for row in rows if row.split("|")[4].strip() == "Verified"]
    summary.append(f"  requirements: {len(verified)} of {len(rows)} verified")
    return 100.0 * len(verified) / len(rows) if rows else 0.0


def current_commit():
    """The commit being measured, e.g. 'iter-1' or 'iter-1-2-g1a2b3c4-dirty'."""
    run = subprocess.run(["git", "describe", "--tags", "--always", "--dirty"],
                         cwd=REPO, capture_output=True, text=True)
    return run.stdout.strip() or "unknown"


def save_to_log(row):
    """Write the row into fom/fom-log.csv, replacing an earlier row of the same iteration."""
    rows = []
    if FOM_LOG.is_file():
        with FOM_LOG.open(newline="", encoding="utf-8") as log:
            rows = [r for r in csv.DictReader(log) if r["iteration"] != str(row["iteration"])]
    rows.append({key: str(value) for key, value in row.items()})
    rows.sort(key=lambda r: int(r["iteration"]))
    with FOM_LOG.open("w", newline="", encoding="utf-8") as log:
        writer = csv.DictWriter(log, fieldnames=LOG_COLUMNS)
        writer.writeheader()
        writer.writerows(rows)


def archive_run(iteration, summary):
    """Copy this run's reports, diagrams and test output to fom/history/iter-N/.

    The reports link to ../diagrams/..., so the folder layout of build/verify/ is kept.
    """
    target = HISTORY / f"iter-{iteration}"
    if target.exists():
        shutil.rmtree(target)
    for folder in ("reports", "diagrams"):
        shutil.copytree(VERIFY_DIR / folder, target / folder)
    if TEST_OUTPUT.is_file():
        shutil.copyfile(TEST_OUTPUT, target / "tests.txt")
    (target / "fom.txt").write_text("\n".join(summary) + "\n", encoding="utf-8")


def write_progress():
    """fom/PROGRESS.md: every logged iteration, with links to its archived reports."""
    with FOM_LOG.open(newline="", encoding="utf-8") as log:
        rows = list(csv.DictReader(log))
    lines = ["# FoM progress", "",
             "Generated by `fom/measure_fom.py --log N` from `fom-log.csv`; do not edit by hand.",
             "FoM = (A + T + R) / 3. A = model alignment, T = test pass rate, "
             "R = requirement coverage (all in %).", "",
             "| Iteration | Date | Commit | A | T | R | FoM | Reports |",
             "|--:|---|---|--:|--:|--:|--:|---|"]
    for row in rows:
        folder = f"history/iter-{row['iteration']}"
        links = [f"[{name.removesuffix('-report.md')}]({folder}/reports/{name})" for name in REPORTS]
        links += [f"[tests]({folder}/tests.txt)", f"[summary]({folder}/fom.txt)"]
        lines.append(f"| {row['iteration']} | {row['date']} | `{row['commit']}` | {row['A']} | "
                     f"{row['T']} | {row['R']} | **{row['FoM']}** | {' · '.join(links)} |")
    PROGRESS.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--log", type=int, metavar="ITERATION",
                        help="log the result as this iteration (fom-log.csv, history/, PROGRESS.md)")
    args = parser.parse_args()

    prepare_verify_project()
    run_verifier()

    summary = []
    a = model_alignment(summary)
    t = test_pass_rate(summary)
    r = requirement_coverage(summary)
    fom = (a + t + r) / 3
    summary.append(f"\n  A = {a:.1f} %   T = {t:.1f} %   R = {r:.1f} %   FoM = {fom:.1f} %")
    print("\n━━ Figure of Merit " + "━" * 47)
    print("\n".join(summary))

    if args.log is not None:
        commit = current_commit()
        summary.insert(0, f"Iteration {args.log}, measured {date.today().isoformat()} at {commit}\n")
        save_to_log({"iteration": args.log, "date": date.today().isoformat(), "commit": commit,
                     "A": f"{a:.1f}", "T": f"{t:.1f}", "R": f"{r:.1f}", "FoM": f"{fom:.1f}",
                     "bench_ns": "-"})
        archive_run(args.log, summary)
        write_progress()
        print(f"  logged as iteration {args.log}: fom/fom-log.csv, fom/history/iter-{args.log}/, "
              f"fom/PROGRESS.md")


if __name__ == "__main__":
    main()
