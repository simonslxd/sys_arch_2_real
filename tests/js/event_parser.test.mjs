// Tests for dashboard/js/event_parser.js. Run in WSL from the repository root:  node --test tests/js/
import { test } from "node:test";
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";

import { parseEventLine } from "../../dashboard/js/event_parser.js";

// The protocol lines of spec/serial-protocol-examples.txt: every line that is not a comment.
const goldenLines = readFileSync(new URL("../../spec/serial-protocol-examples.txt", import.meta.url), "utf8")
  .split("\n")
  .map((line) => line.replace(/\r$/, ""))
  .filter((line) => line !== "" && !line.startsWith("#"));

test("Golden lines: the parser accepts every line the firmware sends (same file as the C++ test)", () => {
  assert.equal(goldenLines.length, 13);
  goldenLines.forEach((line, index) => {
    const event = parseEventLine(line);
    assert.equal(event.seq, index);
  });
});

test("Parser: returns the fields under the protocol's own names", () => {
  assert.deepEqual(parseEventLine("2,LAAN_UT,S001,Multimeter,UTLÅNT,OK"), {
    seq: 2,
    event: "LAAN_UT",
    studentnummer: "S001",
    utstyr_navn: "Multimeter",
    state: "UTLÅNT",
    result: "OK",
  });
});

test("Parser: accepts a trailing \\r or \\n", () => {
  assert.equal(parseEventLine("0,START,S001,Multimeter,LEDIG,OK\r").result, "OK");
  assert.equal(parseEventLine("0,START,S001,Multimeter,LEDIG,OK\n").result, "OK");
});

test("Parser: rejects lines that break the protocol, saying why", () => {
  const broken = [
    ["0,START,S001,Multimeter,LEDIG", /6 felt/],
    ["x,START,S001,Multimeter,LEDIG,OK", /seq/],
    ["0,LAAN,S001,Multimeter,LEDIG,OK", /hendelse/],
    ["0,START,,Multimeter,LEDIG,OK", /studentnummer/],
    ["0,START,S001,,LEDIG,OK", /utstyr_navn/],
    ["0,START,S001,Multimeter,UTLAANT,OK", /tilstand/],
    ["0,START,S001,Multimeter,LEDIG,JA", /resultat/],
  ];
  for (const [line, reason] of broken) {
    assert.throws(() => parseEventLine(line), reason, line);
  }
});
