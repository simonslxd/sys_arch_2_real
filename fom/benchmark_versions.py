#!/usr/bin/env python3
"""Benchmark the domain model of every tagged iteration and draw the result.

    .venv/bin/python fom/benchmark_versions.py                 measure all versions, draw
    .venv/bin/python fom/benchmark_versions.py --chart-only    redraw from the saved CSV

(in WSL, from the repository root)

The benchmark program (benchmarks/) exists only since iteration 6, but the code it measures,
project/impl/, exists since iteration 1. So today's benchmark is built against each iteration's
project/impl/ (taken from its git tag), and all versions are measured in one session on one PC:
in ROUNDS rounds, every version runs once per round, so a slow moment on the PC hits all
versions alike. Each run already reports the median of 7 repetitions of 10 000 loan + return
pairs (benchmarks/lending_benchmark.cpp).

Writes:
  fom/benchmark-versions.csv   every run's numbers, plus median, min and max per version
  fom/benchmark-versions.svg   the chart: median per version, with the min-max range of the runs
"""
import csv
import re
import shutil
import statistics
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
WORK = REPO / "build" / "bench-versions"
CSV_OUT = REPO / "fom" / "benchmark-versions.csv"
SVG_OUT = REPO / "fom" / "benchmark-versions.svg"
ROUNDS = 5
SIZES = [3, 100]   # students and items in the lab, as printed by the benchmark


def tagged_versions():
    """The iteration tags that have a project/impl to benchmark, in order: iter-1, iter-2, ..."""
    tags = subprocess.run(["git", "tag", "--list", "iter-*"], cwd=REPO, capture_output=True,
                          text=True).stdout.split()
    tags.sort(key=lambda tag: int(tag.split("-")[1]))
    return [tag for tag in tags
            if subprocess.run(["git", "cat-file", "-e", f"{tag}:project/impl/CMakeLists.txt"],
                              cwd=REPO, capture_output=True).returncode == 0]


def code_changed(versions):
    """The versions whose measured code (project/impl sources and headers) differs from the
    version before. Between unchanged neighbours, any difference in time is measurement noise."""
    changed = [versions[0]]
    for previous, tag in zip(versions, versions[1:]):
        diff = subprocess.run(["git", "diff", "--quiet", previous, tag, "--",
                               "project/impl/src", "project/impl/include"], cwd=REPO)
        if diff.returncode != 0:
            changed.append(tag)
    return changed


def build(tag):
    """Today's benchmark, built (Release) against the project/impl of `tag`. Returns the program."""
    impl = WORK / tag / "src"
    if (WORK / tag).exists():
        shutil.rmtree(WORK / tag)
    impl.mkdir(parents=True)
    archive = subprocess.run(["git", "archive", tag, "project/impl"], cwd=REPO, capture_output=True,
                             check=True)
    subprocess.run(["tar", "-x", "-C", str(impl)], input=archive.stdout, check=True)
    build_dir = WORK / tag / "build"
    for cmd in (["cmake", "-S", str(REPO / "benchmarks"), "-B", str(build_dir),
                 f"-DLABUTLAAN_IMPL_DIR={impl / 'project' / 'impl'}"],
                ["cmake", "--build", str(build_dir)]):
        subprocess.run(cmd, capture_output=True, check=True)
    return build_dir / "lending_benchmark"


def run_once(program):
    """{lab size: ns per loan + return} from one run of the benchmark."""
    output = subprocess.run([str(program)], capture_output=True, text=True, check=True).stdout
    return {int(size): int(ns) for size, ns in re.findall(r"lab with\s+(\d+) students.*?: (\d+) ns", output)}


def measure(versions):
    """{tag: {size: [ns of every run]}}, the versions alternating in every round."""
    programs = {tag: build(tag) for tag in versions}
    runs = {tag: {size: [] for size in SIZES} for tag in versions}
    for round_number in range(ROUNDS):
        for tag in versions:
            for size, ns in run_once(programs[tag]).items():
                runs[tag][size].append(ns)
        print(f"  round {round_number + 1} of {ROUNDS} done", flush=True)
    return runs


def write_csv(runs):
    with CSV_OUT.open("w", newline="", encoding="utf-8") as out:
        writer = csv.writer(out)
        writer.writerow(["version", "lab_size", *[f"run{i + 1}_ns" for i in range(ROUNDS)],
                         "median_ns", "min_ns", "max_ns"])
        for tag, by_size in runs.items():
            for size in SIZES:
                values = by_size[size]
                writer.writerow([tag, f"{size}x{size}", *values, statistics.median(values),
                                 min(values), max(values)])


# --- the chart ------------------------------------------------------------------------------
# One axis (ns), one line per lab size through the medians, a vertical range per version from
# the fastest to the slowest run. Colours: categorical slots 1 and 2 of the dataviz reference
# palette, validated for light and dark (validate_palette.js: all checks pass). Text uses ink
# colours, never the series colours. Every marker has a tooltip (<title>); the table view is the
# CSV file above.

WIDTH, HEIGHT = 760, 460
LEFT, RIGHT, TOP, BOTTOM = 64, 150, 96, 96

STYLE = """
  .viz { --surface:#fcfcfb; --ink:#0b0b0b; --ink2:#52514e; --muted:#898781; --grid:#e1e0d9;
         --axis:#c3c2b7; --s1:#2a78d6; --s2:#eb6834; font-family:system-ui,-apple-system,"Segoe UI",sans-serif; }
  @media (prefers-color-scheme: dark) {
    .viz { --surface:#1a1a19; --ink:#f0efec; --ink2:#c3c2b7; --muted:#898781; --grid:#2c2c2a;
           --axis:#383835; --s1:#3987e5; --s2:#d95926; }
  }
  .bg { fill:var(--surface); }
  .title { fill:var(--ink); font-size:17px; font-weight:600; }
  .sub, .label { fill:var(--ink2); font-size:13px; }
  .tick { fill:var(--muted); font-size:12px; font-variant-numeric:tabular-nums; }
  .grid { stroke:var(--grid); stroke-width:1; }
  .axis { stroke:var(--axis); stroke-width:1; }
  .line { fill:none; stroke-width:2; stroke-linejoin:round; stroke-linecap:round; }
  .range { stroke-width:2; stroke-linecap:round; opacity:.45; }
  .dot { stroke:var(--surface); stroke-width:2; }
  .s1 { stroke:var(--s1); } .s2 { stroke:var(--s2); }
  .f1 { fill:var(--s1); } .f2 { fill:var(--s2); }
  .hit { fill:transparent; stroke:none; }
"""


def nice_top(value):
    for step in (10, 20, 25, 50, 100, 200, 250, 500):
        if value / step <= 6:
            return step * (int(value // step) + 1), step
    return value, value


def draw_svg(runs, changed):
    versions = list(runs)
    top, step = nice_top(max(max(values) for by_size in runs.values() for values in by_size.values()))
    plot_w, plot_h = WIDTH - LEFT - RIGHT, HEIGHT - TOP - BOTTOM

    def x(i):
        return LEFT + plot_w * (i + 0.5) / len(versions)

    def y(ns):
        return TOP + plot_h * (1 - ns / top)

    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {WIDTH} {HEIGHT}" width="{WIDTH}" '
           f'height="{HEIGHT}" role="img" aria-labelledby="t d" class="viz">',
           f"<style>{STYLE}</style>",
           '<title id="t">Benchmark per iteration: time per loan + return</title>',
           '<desc id="d">Median nanoseconds per Lab::laan_ut + Lab::registrer_retur for each tagged '
           "iteration, for a lab of 3 and of 100 students and items. Numbers: fom/benchmark-versions.csv.</desc>",
           f'<rect class="bg" width="{WIDTH}" height="{HEIGHT}"/>',
           f'<text class="title" x="{LEFT}" y="30">Time per loan + return (ns), by iteration</text>',
           f'<text class="sub" x="{LEFT}" y="50">Median of {ROUNDS} runs; the vertical line spans the '
           "fastest to the slowest run. Lower is faster.</text>"]

    # legend, in ink colours, with a coloured key beside each name
    for n, (size, css) in enumerate(zip(SIZES, ("1", "2"))):
        lx = LEFT + n * 190
        out.append(f'<line class="line s{css}" x1="{lx}" y1="71" x2="{lx + 18}" y2="71"/>'
                   f'<circle class="dot f{css}" cx="{lx + 9}" cy="71" r="4"/>'
                   f'<text class="label" x="{lx + 26}" y="75">lab of {size} × {size}</text>')

    # grid, y ticks, baseline, x labels
    for value in range(0, top + 1, step):
        out.append(f'<line class="grid" x1="{LEFT}" y1="{y(value):.1f}" x2="{LEFT + plot_w}" y2="{y(value):.1f}"/>'
                   f'<text class="tick" x="{LEFT - 8}" y="{y(value) + 4:.1f}" text-anchor="end">{value}</text>')
    out.append(f'<line class="axis" x1="{LEFT}" y1="{y(0):.1f}" x2="{LEFT + plot_w}" y2="{y(0):.1f}"/>')
    for i, tag in enumerate(versions):
        mark = " *" if tag in changed[1:] else ""
        out.append(f'<text class="tick" x="{x(i):.1f}" y="{y(0) + 22:.1f}" text-anchor="middle">{tag}{mark}</text>')
    unchanged = [tag for tag in versions[1:] if tag not in changed]
    out.append(f'<text class="sub" x="{LEFT}" y="{HEIGHT - 34}">* The measured code (project/impl) changed '
               "in this version.</text>")
    if unchanged:
        out.append(f'<text class="sub" x="{LEFT}" y="{HEIGHT - 14}">{", ".join(unchanged)}: same code as the '
                   "version before, so their differences are measurement noise.</text>")

    # per series: ranges, line through the medians, dots with tooltips, end label
    for size, css in zip(SIZES, ("1", "2")):
        medians = [statistics.median(runs[tag][size]) for tag in versions]
        for i, tag in enumerate(versions):
            values = runs[tag][size]
            out.append(f'<line class="range s{css}" x1="{x(i):.1f}" y1="{y(min(values)):.1f}" '
                       f'x2="{x(i):.1f}" y2="{y(max(values)):.1f}"/>')
        points = " ".join(f"{x(i):.1f},{y(m):.1f}" for i, m in enumerate(medians))
        out.append(f'<polyline class="line s{css}" points="{points}"/>')
        for i, tag in enumerate(versions):
            values = runs[tag][size]
            tip = (f"{tag}, lab of {size} × {size}: median {statistics.median(values):g} ns "
                   f"(fastest {min(values)}, slowest {max(values)}; {len(values)} runs)")
            out.append(f'<g><title>{tip}</title><circle class="hit" cx="{x(i):.1f}" cy="{y(medians[i]):.1f}" r="12"/>'
                       f'<circle class="dot f{css}" cx="{x(i):.1f}" cy="{y(medians[i]):.1f}" r="4"/></g>')
        last = len(versions) - 1
        out.append(f'<text class="label" x="{x(last) + 14:.1f}" y="{y(medians[last]) + 4:.1f}">'
                   f"{size} × {size}: {medians[last]:g} ns</text>")

    out.append("</svg>")
    SVG_OUT.write_text("\n".join(out) + "\n", encoding="utf-8")


def read_csv():
    """The runs saved by write_csv(), as {tag: {size: [ns of every run]}}."""
    runs = {}
    with CSV_OUT.open(newline="", encoding="utf-8") as saved:
        for row in csv.DictReader(saved):
            size = int(row["lab_size"].split("x")[0])
            values = [int(row[f"run{i + 1}_ns"]) for i in range(ROUNDS)]
            runs.setdefault(row["version"], {})[size] = values
    return runs


def main():
    if "--chart-only" in sys.argv:   # redraw from the saved numbers, without measuring again
        runs = read_csv()
        draw_svg(runs, code_changed(list(runs)))
        print(f"redrew {SVG_OUT.relative_to(REPO)} from {CSV_OUT.relative_to(REPO)}")
        return
    versions = tagged_versions()
    print(f"benchmarking {', '.join(versions)} ({ROUNDS} rounds)", flush=True)
    runs = measure(versions)
    write_csv(runs)
    draw_svg(runs, code_changed(versions))
    for tag in versions:
        print(f"  {tag}: " + ", ".join(f"{size}x{size} median {statistics.median(runs[tag][size]):g} ns"
                                      for size in SIZES))
    print(f"wrote {CSV_OUT.relative_to(REPO)} and {SVG_OUT.relative_to(REPO)}")


if __name__ == "__main__":
    main()
