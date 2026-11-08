#!/usr/bin/env python3
"""The two final charts: how the FoM and the benchmark changed over the last three iterations.

    .venv/bin/python fom/final_charts.py          (from the repository root; Windows: python -X utf8)

Reads:
  fom/fom-log.csv              the FoM of every logged iteration (fom/measure_fom.py --log N)
  fom/final/benchmark.csv      iterations 6-8 benchmarked in one session:
                               fom/benchmark_versions.py --versions iter-6 iter-7 --worktree iter-8
                                                         --out fom/final/benchmark
Writes:
  fom/final/fom-chart.svg        FoM and its model part A (the UML verifier reports), in %
  fom/final/benchmark-chart.svg  ns per loan + return, median and fastest-slowest run

The benchmark numbers in fom-log.csv (bench_ns) are not used: iterations 6 and 7 were measured
in WSL and iteration 8 on Windows (D-18), each in its own session, so they can't be compared.
The one-session numbers can.

Style as fom/benchmark-versions.svg: one axis, categorical slots 1 and 2 of the dataviz
reference palette (validated in light and dark), ink-coloured text, a legend, labels at the line
ends, a tooltip on every point, and the numbers as a table under the plot.
"""
import csv
import statistics
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
FOM_LOG = REPO / "fom" / "fom-log.csv"
BENCH_CSV = REPO / "fom" / "final" / "benchmark.csv"
OUT = REPO / "fom" / "final"
ITERATIONS = ["6", "7", "8"]                  # as logged in fom-log.csv and tagged in git
LABELS = {"6": "1", "7": "2", "8": "3"}       # as named in the charts: the last three iterations (team choice)
MILESTONES = {"6": "benchmark, SRP review", "7": "“x av y ledig” counts", "8": "final dashboard"}

WIDTH = 760
LEFT, RIGHT, TOP, PLOT_H = 64, 170, 104, 250

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
  .value { fill:var(--ink); font-size:13px; font-weight:600; font-variant-numeric:tabular-nums; }
  .tick { fill:var(--muted); font-size:12px; font-variant-numeric:tabular-nums; }
  .xlab { fill:var(--ink2); font-size:13px; font-weight:600; }
  .th { fill:var(--ink2); font-size:12px; font-weight:600; }
  .td { fill:var(--ink); font-size:12px; font-variant-numeric:tabular-nums; }
  .rule { stroke:var(--grid); stroke-width:1; }
  .grid { stroke:var(--grid); stroke-width:1; }
  .axis { stroke:var(--axis); stroke-width:1; }
  .line { fill:none; stroke-width:2; stroke-linejoin:round; stroke-linecap:round; }
  .range { stroke-width:2; stroke-linecap:round; opacity:.45; }
  .dot { stroke:var(--surface); stroke-width:2; }
  .s1 { stroke:var(--s1); } .s2 { stroke:var(--s2); }
  .f1 { fill:var(--s1); } .f2 { fill:var(--s2); }
  .hit { fill:transparent; stroke:none; }
"""


def esc(text):
    return str(text).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def chart(name, title, subtitle, desc, unit, top, step, series, table, notes):
    """series: [(css slot, legend name, short end-label name, {iteration: (value, low, high)})].
    low/high draw a vertical range (None for no range). table: (header, rows) under the plot."""
    header, rows = table
    table_top = TOP + PLOT_H + 62
    height = table_top + 22 * (len(rows) + 1) + 18 + 20 * len(notes)
    plot_w = WIDTH - LEFT - RIGHT

    def x(i):
        return LEFT + plot_w * (i + 0.5) / len(ITERATIONS)

    def y(value):
        return TOP + PLOT_H * (1 - value / top)

    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {WIDTH} {height}" width="{WIDTH}" '
           f'height="{height}" role="img" aria-labelledby="t d" class="viz">',
           f"<style>{STYLE}</style>",
           f'<title id="t">{esc(title)}</title>', f'<desc id="d">{esc(desc)}</desc>',
           f'<rect class="bg" width="{WIDTH}" height="{height}"/>',
           f'<text class="title" x="{LEFT}" y="30">{esc(title)}</text>',
           f'<text class="sub" x="{LEFT}" y="50">{esc(subtitle)}</text>']

    for n, (css, legend, _, _) in enumerate(series):   # legend: coloured key, ink text
        lx = LEFT + n * 270
        out.append(f'<line class="line s{css}" x1="{lx}" y1="73" x2="{lx + 18}" y2="73"/>'
                   f'<circle class="dot f{css}" cx="{lx + 9}" cy="73" r="4"/>'
                   f'<text class="label" x="{lx + 26}" y="77">{esc(legend)}</text>')

    for value in range(0, top + 1, step):
        out.append(f'<line class="grid" x1="{LEFT}" y1="{y(value):.1f}" x2="{LEFT + plot_w}" y2="{y(value):.1f}"/>'
                   f'<text class="tick" x="{LEFT - 8}" y="{y(value) + 4:.1f}" text-anchor="end">{value}</text>')
    out.append(f'<text class="tick" x="{LEFT - 8}" y="{TOP - 12}" text-anchor="end">{esc(unit)}</text>')
    out.append(f'<line class="axis" x1="{LEFT}" y1="{y(0):.1f}" x2="{LEFT + plot_w}" y2="{y(0):.1f}"/>')
    for i, it in enumerate(ITERATIONS):
        out.append(f'<text class="xlab" x="{x(i):.1f}" y="{y(0) + 22:.1f}" text-anchor="middle">Iteration {LABELS[it]}</text>'
                   f'<text class="tick" x="{x(i):.1f}" y="{y(0) + 38:.1f}" text-anchor="middle">{esc(MILESTONES[it])}</text>')

    end_labels = []
    for css, legend, short, values in series:
        for i, it in enumerate(ITERATIONS):
            _, low, high = values[it]
            if low is not None:
                out.append(f'<line class="range s{css}" x1="{x(i):.1f}" y1="{y(low):.1f}" '
                           f'x2="{x(i):.1f}" y2="{y(high):.1f}"/>')
        points = " ".join(f"{x(i):.1f},{y(values[it][0]):.1f}" for i, it in enumerate(ITERATIONS))
        out.append(f'<polyline class="line s{css}" points="{points}"/>')
        for i, it in enumerate(ITERATIONS):
            value, low, high = values[it]
            tip = f"Iteration {LABELS[it]}, {legend}: {value:g} {unit}"
            if low is not None:
                tip += f" (fastest run {low:g}, slowest {high:g})"
            out.append(f'<g><title>{esc(tip)}</title>'
                       f'<circle class="hit" cx="{x(i):.1f}" cy="{y(value):.1f}" r="12"/>'
                       f'<circle class="dot f{css}" cx="{x(i):.1f}" cy="{y(value):.1f}" r="5"/></g>')
        last = values[ITERATIONS[-1]][0]
        end_labels.append([y(last) + 4, f"{short}: {last:g} {unit}"])
    end_labels.sort()
    for k in range(1, len(end_labels)):                  # keep end labels apart
        end_labels[k][0] = max(end_labels[k][0], end_labels[k - 1][0] + 16)
    for ly, text in end_labels:
        out.append(f'<text class="value" x="{x(len(ITERATIONS) - 1) + 16:.1f}" y="{ly:.1f}">{esc(text)}</text>')

    # the numbers as a table
    gap = min(150, (WIDTH - LEFT - 150 - 130) // max(1, len(header) - 2))   # the last column ~130 px wide
    columns = [LEFT] + [LEFT + 150 + gap * k for k in range(len(header) - 1)]
    out.append(f'<line class="rule" x1="{LEFT}" y1="{table_top - 16}" x2="{WIDTH - 24}" y2="{table_top - 16}"/>')
    for cx, text in zip(columns, header):
        out.append(f'<text class="th" x="{cx}" y="{table_top}">{esc(text)}</text>')
    for r, row in enumerate(rows):
        ry = table_top + 22 * (r + 1)
        for cx, text in zip(columns, row):
            out.append(f'<text class="td" x="{cx}" y="{ry}">{esc(text)}</text>')
    for n, note in enumerate(notes):
        out.append(f'<text class="sub" x="{LEFT}" y="{table_top + 22 * (len(rows) + 1) + 14 + 20 * n}">{esc(note)}</text>')
    out.append("</svg>")
    (OUT / name).write_text("\n".join(out) + "\n", encoding="utf-8")
    return OUT / name


def fom_chart():
    with FOM_LOG.open(newline="", encoding="utf-8") as log:
        rows = {r["iteration"]: r for r in csv.DictReader(log)}
    fom = {it: (float(rows[it]["FoM"]), None, None) for it in ITERATIONS}
    a = {it: (float(rows[it]["A"]), None, None) for it in ITERATIONS}
    table = (["Iteration", "FoM", "A (UML reports)", "T (tests)", "R (requirements)"],
             [[f"{LABELS[it]}  ({rows[it]['date']})", f"{rows[it]['FoM']} %", f"{rows[it]['A']} %",
               f"{rows[it]['T']} %", f"{rows[it]['R']} %"] for it in ITERATIONS])
    first, last = float(rows[ITERATIONS[0]]["FoM"]), float(rows[ITERATIONS[-1]]["FoM"])
    return chart(
        "fom-chart.svg", "Figure of Merit, iterations 1 to 3",
        "FoM = (A + T + R) / 3, in %. Higher is better.",
        "FoM and its model-alignment part A for iterations 1, 2 and 3 (the last three iterations), "
        "from fom/fom-log.csv.",
        "%", 100, 20,
        [("1", "FoM", "FoM", fom), ("2", "A: model alignment (UML verifier)", "A", a)],
        table,
        [f"FoM {first:g} % → {last:g} % ({last - first:+.1f} points). A fell in iteration 2: two getters for the "
         "counts are not in the class diagram (DEV-17).",
         "Iteration 3 changed only the dashboard, so all three reports are unchanged. T and R stayed at 100 %."])


def benchmark_chart():
    runs = {}
    with BENCH_CSV.open(newline="", encoding="utf-8") as saved:
        for row in csv.DictReader(saved):
            values = [int(row[k]) for k in row if k.startswith("run")]
            runs.setdefault(row["version"].removeprefix("iter-"), {})[row["lab_size"]] = values

    def series(size):
        return {it: (statistics.median(runs[it][size]), min(runs[it][size]), max(runs[it][size]))
                for it in ITERATIONS}

    big, small = series("100x100"), series("3x3")
    top = 10 * (int(max(v[2] for v in big.values()) // 10) + 2)
    table = (["Iteration", "100 × 100 lab", "3 × 3 lab"],
             [[LABELS[it], f"{big[it][0]:g} ns ({big[it][1]}–{big[it][2]})",
               f"{small[it][0]:g} ns ({small[it][1]}–{small[it][2]})"]
              for it in ITERATIONS])
    n = len(next(iter(runs.values()))["100x100"])
    return chart(
        "benchmark-chart.svg", "Benchmark, iterations 1 to 3: time per loan + return",
        f"Median of {n} runs, all measured in one session; the vertical line spans the fastest to the "
        "slowest run. Lower is faster.",
        "Median nanoseconds per Lab::laan_ut + Lab::registrer_retur for iterations 1, 2 and 3 (the last "
        "three iterations), in a lab of 100 and of 3 students and items. Numbers: fom/final/benchmark.csv.",
        "ns", top, 10,
        [("1", "lab of 100 students × 100 items", "100 × 100", big),
         ("2", "lab of 3 students × 3 items", "3 × 3", small)],
        table,
        ["Iteration 2 added two getters that loans and returns don't call; iteration 3 changed no domain code.",
         "So the differences are measurement noise. Measured 2026-11-08 on Windows (MSYS2, D-18)."])


if __name__ == "__main__":
    for path in (fom_chart(), benchmark_chart()):
        print(f"wrote {path.relative_to(REPO)}")
