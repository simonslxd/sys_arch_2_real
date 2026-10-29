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

test("CSV row: the receive time, then the protocol fields in protocol order, counts included", () => {
  const event = parseEventLine("7,LAAN_UT,S002,Multimeter,UTLÅNT,FEIL,0,2");
  assert.equal(toCsvRow(event, "2026-10-29T14:05:12.345Z"),
               "2026-10-29T14:05:12.345Z,7,LAAN_UT,S002,Multimeter,UTLÅNT,FEIL,0,2\n");
});

test("Round trip: the 16 golden lines written as CSV read back as the same events and times", () => {
  const events = goldenLines.map(parseEventLine);
  const times = events.map((_, i) => new Date(Date.UTC(2026, 9, 29, 14, 0, i)).toISOString());
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

test("Reading: a file from iteration 6 (no counts) is refused with a clear message", () => {
  const oldFile = "mottatt,seq,event,studentnummer,utstyr_navn,state,result\n" +
                  "2026-10-28T12:00:33.466Z,2,LAAN_UT,S001,Oscilloskop,UTLÅNT,OK\n";
  assert.throws(() => parseCsv(oldFile), {
    message: "Gammelt filformat uten antall (iterasjon 6). Lag en ny CSV-fil.",
  });
});

test("Reading: the real iteration-6 test file is refused too", () => {
  const oldFile = readFileSync(new URL("../hardware/logs/2026-10-28-dashboard-run2.csv", import.meta.url), "utf8");
  assert.throws(() => parseCsv(oldFile), /Gammelt filformat/);
});

test("Reading: bad lines are reported with their line number, good lines are kept", () => {
  const text = [
    CSV_HEADER,
    "2026-10-29T14:00:00.000Z,0,START,S001,Multimeter,LEDIG,OK,2,2",
    "i går,1,VELG,S002,Multimeter,LEDIG,OK,2,2",
    "2026-10-29T14:00:02.000Z,2,LAAN,S002,Multimeter,LEDIG,OK,2,2",
    "2026-10-29T14:00:03.000Z,3,LAAN_UT,S002,Multimeter,LEDIG,OK,1,2",
  ].join("\n");
  const { rows, rejected } = parseCsv(text);
  assert.deepEqual(rows.map((row) => row.seq), [0, 3]);
  assert.deepEqual(rejected.map((bad) => bad.lineNumber), [3, 4]);
  assert.match(rejected[0].reason, /mottatt/);
  assert.match(rejected[1].reason, /hendelse/);
});

test("Reading: Windows line ends (\\r\\n) are accepted", () => {
  const text = CSV_HEADER + "\r\n2026-10-29T14:00:00.000Z,0,START,S001,Multimeter,LEDIG,OK,2,2\r\n";
  assert.equal(parseCsv(text).rows.length, 1);
});

test("Logging: every LAAN_UT and RETUR is logged, OK and FEIL; START, VELG and STATUS are not", () => {
  const logged = goldenLines.map(parseEventLine).filter(isLogged);
  assert.deepEqual(logged.map((event) => `${event.event} ${event.result}`), [
    "LAAN_UT OK", "LAAN_UT OK", "LAAN_UT FEIL", "RETUR FEIL", "RETUR OK", "LAAN_UT OK",
  ]);
});
