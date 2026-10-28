// Tests for LineBuffer in dashboard/js/serial_link.js. Run in WSL from the repository root:  node --test tests/js/
import { test } from "node:test";
import assert from "node:assert/strict";

import { LineBuffer } from "../../dashboard/js/serial_link.js";

test("LineBuffer: one chunk with two lines gives both", () => {
  const buffer = new LineBuffer();
  assert.deepEqual(buffer.push("0,START\n1,VELG\n"), ["0,START", "1,VELG"]);
});

test("LineBuffer: a line split over several chunks comes out once it is complete", () => {
  const buffer = new LineBuffer();
  assert.deepEqual(buffer.push("2,LAAN_"), []);
  assert.deepEqual(buffer.push("UT,S001"), []);
  assert.deepEqual(buffer.push(",OK\n3,"), ["2,LAAN_UT,S001,OK"]);
  assert.deepEqual(buffer.push("VELG\n"), ["3,VELG"]);
});

test("LineBuffer: \\r\\n line ends and empty lines are removed", () => {
  const buffer = new LineBuffer();
  assert.deepEqual(buffer.push("a\r\n\r\n\nb\n"), ["a", "b"]);
});
