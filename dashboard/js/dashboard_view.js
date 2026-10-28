// Draws the dashboard. This is the only module that touches the page (the DOM).

const EVENT_LABELS = {
  START: "stasjonen startet",
  LAAN_UT: "utlån",
  RETUR: "retur",
  VELG: "nytt valg",
};

function cell(row, text, className) {
  const td = row.insertCell();
  td.textContent = text;
  if (className) {
    td.className = className;
  }
  return td;
}

export function showStatus(text, kind) {
  const status = document.getElementById("status");
  status.textContent = text;
  status.className = `status ${kind}`;   // kind: "frakoblet", "tilkoblet" or "feil"
}

export function setConnected(connected) {
  document.getElementById("koble-til").disabled = connected;
  document.getElementById("koble-fra").disabled = !connected;
}

// The current selection on the station: every event carries it.
export function showSelection(event) {
  document.getElementById("valgt-student").textContent = event.studentnummer;
  document.getElementById("valgt-utstyr").textContent = event.utstyr_navn;
}

// One row per item in "Utstyrsstatus"; an event updates the row of the item it names.
export function showItemState(event) {
  const table = document.getElementById("utstyrsstatus");
  let row = table.querySelector(`tr[data-utstyr="${CSS.escape(event.utstyr_navn)}"]`);
  if (!row) {
    row = table.tBodies[0].insertRow();
    row.dataset.utstyr = event.utstyr_navn;
    cell(row, event.utstyr_navn);
    cell(row, "");
  }
  const stateCell = row.cells[1];
  stateCell.textContent = event.state;
  stateCell.className = event.state === "LEDIG" ? "ledig" : "utlaant";
}

// Newest event first.
export function showEvent(event) {
  const body = document.getElementById("hendelser").tBodies[0];
  const row = body.insertRow(0);
  cell(row, String(event.seq));
  cell(row, `${event.event} (${EVENT_LABELS[event.event]})`);
  cell(row, event.studentnummer);
  cell(row, event.utstyr_navn);
  cell(row, event.state, event.state === "LEDIG" ? "ledig" : "utlaant");
  cell(row, event.result, event.result === "OK" ? "ok" : "feil");
}

// A line that is not valid protocol: shown, so a mismatch with the specification is visible.
export function showRejectedLine(line, reason) {
  const section = document.getElementById("avviste");
  section.hidden = false;
  const item = document.createElement("li");
  item.textContent = `"${line}": ${reason}`;
  section.querySelector("ul").prepend(item);
}
