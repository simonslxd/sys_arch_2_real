// Draws the dashboard. This is the only module that touches the page (the DOM).

function cell(row, text, className) {
  const td = row.insertCell();
  td.textContent = text;
  if (className) {
    td.className = className;
  }
  return td;
}

function stateClass(state) {
  return state === "LEDIG" ? "ledig" : "utlaant";
}

// "1 av 2 ledig": available out of total (REQ-UI-02).
function countText(event) {
  return `${event.antall_tilgjengelig} av ${event.antall_totalt} ledig`;
}

// kind: "frakoblet", "tilkoblet" or "feil"
export function showStatus(text, kind) {
  const status = document.getElementById("status");
  status.textContent = text;
  status.className = `status ${kind}`;
}

// kind: "frakoblet" (no file yet), "tilkoblet" (storing) or "feil"
export function showStorageStatus(text, kind) {
  const status = document.getElementById("lagring-status");
  status.textContent = text;
  status.className = `status ${kind}`;
}

// The buttons that may be used: connecting needs a chosen file; the file can only be changed
// while disconnected.
export function setButtons({ fileChosen, connected, supported }) {
  document.getElementById("ny-fil").disabled = connected || !supported.files;
  document.getElementById("apne-fil").disabled = connected || !supported.files;
  document.getElementById("koble-til").disabled = connected || !fileChosen || !supported.serial;
  document.getElementById("koble-fra").disabled = !connected;
}

// Empties every table, before the content of a newly chosen file is shown.
export function clearEvents() {
  document.getElementById("hendelser").tBodies[0].replaceChildren();
  document.getElementById("utstyrsstatus").tBodies[0].replaceChildren();
  document.getElementById("valgt-student").textContent = "–";
  document.getElementById("valgt-utstyr").textContent = "–";
  const rejected = document.getElementById("avviste");
  rejected.querySelector("ul").replaceChildren();
  rejected.hidden = true;
}

// The current selection on the station: every event carries it.
export function showSelection(event) {
  document.getElementById("valgt-student").textContent = event.studentnummer;
  document.getElementById("valgt-utstyr").textContent = event.utstyr_navn;
}

// One row per item in "Utstyrsstatus"; an event updates the row of the item it names: how many
// are available (REQ-UI-02) and the state.
export function showItemState(event) {
  const table = document.getElementById("utstyrsstatus");
  let row = table.querySelector(`tr[data-utstyr="${CSS.escape(event.utstyr_navn)}"]`);
  if (!row) {
    row = table.tBodies[0].insertRow();
    row.dataset.utstyr = event.utstyr_navn;
    cell(row, event.utstyr_navn);
    cell(row, "", "antall");
    cell(row, "");
  }
  row.cells[1].textContent = countText(event);
  row.cells[2].textContent = event.state;
  row.cells[2].className = stateClass(event.state);
}

// Newest event first. source: "fil" (read from the CSV file) or "live" (just received).
export function showEvent(event, mottatt, source) {
  const row = document.getElementById("hendelser").tBodies[0].insertRow(0);
  row.className = source === "fil" ? "fra-fil" : "";
  cell(row, new Date(mottatt).toLocaleString("nb-NO"));
  cell(row, source);
  cell(row, String(event.seq));
  cell(row, event.event);
  cell(row, event.studentnummer);
  cell(row, event.utstyr_navn);
  cell(row, event.state, stateClass(event.state));
  cell(row, countText(event));
  cell(row, event.result, event.result === "OK" ? "ok" : "feil");
}

// Anything that does not follow the protocol is listed, so it is visible: an invalid line, or
// lines missing from the micro:bit.
function showProblem(text) {
  const section = document.getElementById("avviste");
  section.hidden = false;
  const item = document.createElement("li");
  item.textContent = text;
  section.querySelector("ul").prepend(item);
}

export function showRejectedLine(line, reason) {
  showProblem(`"${line}": ${reason}`);
}

export function showMissingLines(count, seq) {
  showProblem(`${count} linje(r) fra micro:bit mangler før seq ${seq}.`);
}
