// Wires the dashboard together and keeps the session's state (connected or not, the last seq):
//   CSV file    -> stored loans and returns -> page
//   connect     -> send the status request; each STATUS answer -> that item's row on the page
//   serial line -> parsed event -> selection and item state on the page; a loan or return is
//                  also written to the CSV file (with the time it arrived) and listed
// The rules themselves live in the tested modules: what is stored (csv_format.isLogged), what a
// line means (event_parser), and when lines are missing (event_parser.missingLines).
import { isLogged, toCsvRow } from "./csv_format.js";
import { CsvStore, isFileAccessSupported } from "./csv_store.js";
import { missingLines, parseEventLine } from "./event_parser.js";
import { SerialLink, isWebSerialSupported } from "./serial_link.js";
import * as view from "./dashboard_view.js";

const supported = { serial: isWebSerialSupported(), files: isFileAccessSupported() };
const store = new CsvStore();
let connected = false;
let previousSeq = null;   // seq of the last line received since connecting

function updateButtons() {
  view.setButtons({ fileChosen: store.fileName !== null, connected, supported });
}

function showOnPage(event, mottatt, source) {
  view.showEvent(event, mottatt, source);
  view.showSelection(event);
  view.showItemState(event);
}

// --- the CSV file -------------------------------------------------------------------------

async function useFile(choose) {
  try {
    const { rows, rejected } = await choose();
    view.clearEvents();
    rows.forEach((row) => showOnPage(row, row.mottatt, "fil"));
    rejected.forEach((bad) => view.showRejectedLine(bad.line, `CSV-linje ${bad.lineNumber}: ${bad.reason}`));
    view.showStorageStatus(`Lagrer i ${store.fileName} (${rows.length} lagrede hendelser lest inn).`, "tilkoblet");
  } catch (error) {
    if (error.name !== "AbortError") {   // AbortError: the user closed the file dialog
      view.showStorageStatus(`Kunne ikke bruke filen: ${error.message}`, "feil");
    }
  }
  updateButtons();
}

document.getElementById("ny-fil").addEventListener("click", () => useFile(() => store.createNew()));
document.getElementById("apne-fil").addEventListener("click", () => useFile(() => store.openExisting()));

// --- the micro:bit ------------------------------------------------------------------------

function handleLine(line) {
  let event;
  try {
    event = parseEventLine(line);
  } catch (error) {
    view.showRejectedLine(line, error.message);
    return;
  }
  const missing = missingLines(previousSeq, event.seq);
  if (missing > 0) {
    view.showMissingLines(missing, event.seq);
  }
  previousSeq = event.seq;

  if (event.event === "STATUS") {
    view.showItemState(event);   // the answer to our status request: the item table only
    return;
  }
  if (!isLogged(event)) {
    view.showSelection(event);
    view.showItemState(event);
    return;
  }
  const mottatt = new Date().toISOString();
  store.append(toCsvRow(event, mottatt)).catch((error) => {
    view.showStorageStatus(`Kunne ikke skrive til ${store.fileName}: ${error.message}`, "feil");
  });
  showOnPage(event, mottatt, "live");
}

function handleClosed(error) {
  connected = false;
  updateButtons();
  if (error) {
    view.showStatus(`Forbindelsen ble brutt: ${error.message}`, "feil");
  } else {
    view.showStatus("Frakoblet.", "frakoblet");
  }
}

const link = new SerialLink(handleLine, handleClosed);

document.getElementById("koble-til").addEventListener("click", async () => {
  try {
    previousSeq = null;   // lines sent while disconnected are not counted as missing
    await link.connect();
    connected = true;
    // The micro:bit's START line went out before we listened: ask for every item's status (REQ-COM-03).
    await link.send("STATUS\n");
    view.showStatus("Tilkoblet micro:bit. Venter på hendelser …", "tilkoblet");
  } catch (error) {
    if (error.name !== "NotFoundError") {   // NotFoundError: the user closed the port chooser
      view.showStatus(`Kunne ikke koble til: ${error.message}`, "feil");
    }
  }
  updateButtons();
});

document.getElementById("koble-fra").addEventListener("click", () => link.disconnect());

// --- start --------------------------------------------------------------------------------

updateButtons();
if (!supported.serial || !supported.files) {
  view.showStatus("Denne nettleseren støtter ikke WebSerial eller filtilgang. Bruk Chrome eller Edge.", "feil");
}
