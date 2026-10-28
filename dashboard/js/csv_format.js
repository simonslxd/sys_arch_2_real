// The CSV file format of the stored events. No browser features are used here, so the tests run
// in Node.
//
// The first line is the header. Every other line is the time the dashboard received the event
// (ISO 8601, from the PC's clock) followed by the protocol line exactly as the micro:bit sent it:
//
//   mottatt,seq,event,studentnummer,utstyr_navn,state,result
//   2026-10-28T14:05:12.345Z,7,LAAN_UT,S002,Multimeter,UTLÅNT,OK
//
// Protocol fields never contain commas or line breaks (spec/serial-protocol.md), so no CSV
// quoting is needed, and a stored line is checked with the same parser as a live line.
import { parseEventLine } from "./event_parser.js";

export const CSV_HEADER = "mottatt,seq,event,studentnummer,utstyr_navn,state,result";

// Only loans and returns are logged, both OK and FEIL; START and VELG are not (team decision,
// REQ-STO-01).
export const LOGGED_EVENTS = ["LAAN_UT", "RETUR"];

export function isLogged(event) {
  return LOGGED_EVENTS.includes(event.event);
}

export function toCsvRow(event, mottatt) {
  return [mottatt, event.seq, event.event, event.studentnummer, event.utstyr_navn, event.state, event.result]
    .join(",") + "\n";
}

// Reads a whole CSV file. Returns { rows, rejected }:
//   rows      the valid events in file order, each with its `mottatt` time
//   rejected  { lineNumber, line, reason } for every line that is not a valid event
// An empty file has no rows. A file whose first line is not the header is not an event file:
// that throws an Error, so the dashboard never appends to someone else's CSV file.
export function parseCsv(text) {
  const lines = text.split("\n").map((line) => line.replace(/\r$/, ""));
  if (lines.every((line) => line === "")) {
    return { rows: [], rejected: [] };
  }
  if (lines[0] !== CSV_HEADER) {
    throw new Error(`ikke en hendelsesfil: første linje skal være "${CSV_HEADER}"`);
  }

  const rows = [];
  const rejected = [];
  lines.forEach((line, index) => {
    if (index === 0 || line === "") {
      return;
    }
    const comma = line.indexOf(",");
    const mottatt = line.slice(0, comma);
    try {
      if (comma < 0 || Number.isNaN(Date.parse(mottatt))) {
        throw new Error(`mottatt er ikke et tidspunkt: "${mottatt}"`);
      }
      rows.push({ mottatt, ...parseEventLine(line.slice(comma + 1)) });
    } catch (error) {
      rejected.push({ lineNumber: index + 1, line, reason: error.message });
    }
  });
  return { rows, rejected };
}
