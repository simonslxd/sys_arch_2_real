// Tests for dashboard/js/event_parser.js. Run in WSL from the repository root:  node --test tests/js/
import { test } from "node:test";
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";

import { missingLines, parseEventLine } from "../../dashboard/js/event_parser.js";

// The protocol lines of spec/serial-protocol-examples.txt: every line that is not a comment.
const goldenLines = readFileSync(new URL("../../spec/serial-protocol-examples.txt", import.meta.url), "utf8")
  .split("\n")
  .map((line) => line.replace(/\r$/, ""))
  .filter((line) => line !== "" && !line.startsWith("#"));

test("Golden lines: the parser accepts every line the firmware sends (same file as the C++ test)", () => {
  assert.equal(goldenLines.length, 16);
  goldenLines.forEach((line, index) => {
    const event = parseEventLine(line);
    assert.equal(event.seq, index);
  });
});

test("Parser: returns the fields under the protocol's own names, counts as numbers", () => {
  assert.deepEqual(parseEventLine("2,LAAN_UT,S001,Multimeter,UTLÅNT,OK,0,2"), {
    seq: 2,
    event: "LAAN_UT",
    studentnummer: "S001",
    utstyr_navn: "Multimeter",
    state: "UTLÅNT",
    result: "OK",
    antall_tilgjengelig: 0,
    antall_totalt: 2,
  });
});

test("Parser: the golden STATUS lines give every item's count (REQ-COM-03)", () => {
  const status = goldenLines.map(parseEventLine).filter((event) => event.event === "STATUS");
  assert.deepEqual(status.map((event) => `${event.utstyr_navn} ${event.antall_tilgjengelig}/${event.antall_totalt}`),
                   ["Multimeter 1/2", "Oscilloskop 0/1", "Loddestasjon 3/3"]);
});

test("Parser: accepts a trailing \\r or \\n", () => {
  assert.equal(parseEventLine("0,START,S001,Multimeter,LEDIG,OK,2,2\r").result, "OK");
  assert.equal(parseEventLine("0,START,S001,Multimeter,LEDIG,OK,2,2\n").result, "OK");
});

test("Parser: rejects lines that break the protocol, saying why", () => {
  const broken = [
    ["0,START,S001,Multimeter,LEDIG,OK", /8 felt/],   // a version 1 line, without counts
    ["x,START,S001,Multimeter,LEDIG,OK,2,2", /seq/],
    ["0,LAAN,S001,Multimeter,LEDIG,OK,2,2", /hendelse/],
    ["0,START,,Multimeter,LEDIG,OK,2,2", /studentnummer/],
    ["0,START,S001,,LEDIG,OK,2,2", /utstyr_navn/],
    ["0,START,S001,Multimeter,UTLAANT,OK,2,2", /tilstand/],
    ["0,START,S001,Multimeter,LEDIG,JA,2,2", /resultat/],
    ["0,START,S001,Multimeter,LEDIG,OK,to,2", /antall/],
    ["0,START,S001,Multimeter,LEDIG,OK,-1,2", /antall/],
    ["0,START,S001,Multimeter,LEDIG,OK,3,2", /flere ledige enn totalt/],
  ];
  for (const [line, reason] of broken) {
    assert.throws(() => parseEventLine(line), reason, line);
  }
});

test("Missing lines: a jump in seq counts the lines in between", () => {
  assert.equal(missingLines(85, 86), 0);
  assert.equal(missingLines(85, 88), 2);
});

test("Missing lines: nothing is missing before the first line or after a restart (seq 0)", () => {
  assert.equal(missingLines(null, 63), 0);
  assert.equal(missingLines(85, 0), 0);
});
