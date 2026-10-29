// Parses one line of the serial protocol, version 2 (spec/serial-protocol.md):
//
//   <seq>,<event>,<studentnummer>,<utstyr_navn>,<state>,<result>,<antall_tilgjengelig>,<antall_totalt>
//
// The returned object uses the protocol's own field names, so every value can be traced back
// to the specification. No browser features are used here, so the tests run in Node.

export const EVENTS = ["START", "LAAN_UT", "RETUR", "VELG", "STATUS"];
export const STATES = ["LEDIG", "UTLÅNT"];
export const RESULTS = ["OK", "FEIL"];
const FIELD_COUNT = 8;

// Returns { seq, event, studentnummer, utstyr_navn, state, result, antall_tilgjengelig,
// antall_totalt }, or throws an Error whose message (in Norwegian, it is shown on the dashboard)
// says what is wrong with the line.
export function parseEventLine(line) {
  const fields = line.replace(/\r?\n?$/, "").split(",");
  if (fields.length !== FIELD_COUNT) {
    throw new Error(`forventet ${FIELD_COUNT} felt, fikk ${fields.length}`);
  }
  const [seqText, event, studentnummer, utstyr_navn, state, result, availableText, totalText] = fields;

  if (!/^\d+$/.test(seqText)) {
    throw new Error(`seq er ikke et tall: "${seqText}"`);
  }
  if (!EVENTS.includes(event)) {
    throw new Error(`ukjent hendelse: "${event}"`);
  }
  if (studentnummer === "") {
    throw new Error("studentnummer mangler");
  }
  if (utstyr_navn === "") {
    throw new Error("utstyr_navn mangler");
  }
  if (!STATES.includes(state)) {
    throw new Error(`ukjent tilstand: "${state}"`);
  }
  if (!RESULTS.includes(result)) {
    throw new Error(`ukjent resultat: "${result}"`);
  }
  if (!/^\d+$/.test(availableText) || !/^\d+$/.test(totalText)) {
    throw new Error(`antall er ikke et tall: "${availableText}" av "${totalText}"`);
  }
  const antall_tilgjengelig = Number(availableText);
  const antall_totalt = Number(totalText);
  if (antall_tilgjengelig > antall_totalt) {
    throw new Error(`flere ledige enn totalt: ${antall_tilgjengelig} av ${antall_totalt}`);
  }
  return { seq: Number(seqText), event, studentnummer, utstyr_navn, state, result, antall_tilgjengelig, antall_totalt };
}

// How many lines are missing between two received lines: seq counts every line the micro:bit
// sends, so a jump means lost lines. 0 when nothing is missing, when this is the first line
// (previousSeq is null), or when the micro:bit restarted (seq 0).
export function missingLines(previousSeq, seq) {
  if (previousSeq === null || seq === 0 || seq <= previousSeq) {
    return 0;
  }
  return seq - previousSeq - 1;
}
