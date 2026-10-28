// Tests for dashboard/js/csv_format.js. Run in WSL from the repository root:  node --test tests/js/
import { test } from "node:test";
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";

import { CSV_HEADER, isLogged, parseCsv, toCsvRow } from "../../dashboard/js/csv_format.js";
import { parseEventLine } from "../../dashboard/js/event_parser.js";

const goldenLines = readFileSync(new URL("../../spec/serial-protocol-examples.txt", import.meta.url), "utf8")
  .split("\n")
  .map((line) => line.replace(/\r$/, ""))
  .filter((line) => line !== "" && !line.startsWith("#"));

test("CSV row: the receive time, then the protocol fields in protocol order", () => {
  const event = parseEventLine("7,LAAN_UT,S002,Multimeter,UTLÅNT,FEIL");
  assert.equal(toCsvRow(event, "2026-10-28T14:05:12.345Z"),
               "2026-10-28T14:05:12.345Z,7,LAAN_UT,S002,Multimeter,UTLÅNT,FEIL\n");
});

test("Round trip: the 13 golden lines written as CSV read back as the same events and times", () => {
  const events = goldenLines.map(parseEventLine);
  const times = events.map((_, i) => new Date(Date.UTC(2026, 9, 28, 14, 0, i)).toISOString());
  const text = CSV_HEADER + "\n" + events.map((event, i) => toCsvRow(event, times[i])).join("");

  const { rows, rejected } = parseCsv(text);
  assert.equal(rejected.length, 0);
  assert.deepEqual(rows, events.map((event, i) => ({ mottatt: times[i], ...event })));
});

test("Reading: an empty file has no rows", () => {
  assert.deepEqual(parseCsv(""), { rows: [], rejected: [] });
  assert.deepEqual(parseCsv(CSV_HEADER + "\n"), { rows: [], rejected: [] });
});

test("Reading: a file that does not start with the header is refused", () => {
  assert.throws(() => parseCsv("navn,alder\nOla,20\n"), /ikke en hendelsesfil/);
});

test("Reading: bad lines are reported with their line number, good lines are kept", () => {
  const text = [
    CSV_HEADER,
    "2026-10-28T14:00:00.000Z,0,START,S001,Multimeter,LEDIG,OK",
    "i går,1,VELG,S002,Multimeter,LEDIG,OK",
    "2026-10-28T14:00:02.000Z,2,LAAN,S002,Multimeter,LEDIG,OK",
    "2026-10-28T14:00:03.000Z,3,LAAN_UT,S002,Multimeter,UTLÅNT,OK",
  ].join("\n");
  const { rows, rejected } = parseCsv(text);
  assert.deepEqual(rows.map((row) => row.seq), [0, 3]);
  assert.deepEqual(rejected.map((bad) => bad.lineNumber), [3, 4]);
  assert.match(rejected[0].reason, /mottatt/);
  assert.match(rejected[1].reason, /hendelse/);
});

test("Reading: Windows line ends (\\r\\n) are accepted", () => {
  const text = CSV_HEADER + "\r\n2026-10-28T14:00:00.000Z,0,START,S001,Multimeter,LEDIG,OK\r\n";
  assert.equal(parseCsv(text).rows.length, 1);
});

test("Logging: every LAAN_UT and RETUR is logged, OK and FEIL; START and VELG are not", () => {
  const logged = goldenLines.map(parseEventLine).filter(isLogged);
  assert.deepEqual(logged.map((event) => `${event.event} ${event.result}`), [
    "LAAN_UT OK", "LAAN_UT OK", "LAAN_UT FEIL", "RETUR FEIL", "RETUR OK", "LAAN_UT OK",
  ]);
});
