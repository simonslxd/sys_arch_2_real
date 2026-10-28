// Wires the dashboard together: serial line -> parsed event -> page. No logic of its own.
import { parseEventLine } from "./event_parser.js";
import { SerialLink, isWebSerialSupported } from "./serial_link.js";
import * as view from "./dashboard_view.js";

function handleLine(line) {
  let event;
  try {
    event = parseEventLine(line);
  } catch (error) {
    view.showRejectedLine(line, error.message);
    return;
  }
  view.showEvent(event);
  view.showSelection(event);
  view.showItemState(event);
}

function handleClosed(error) {
  view.setConnected(false);
  if (error) {
    view.showStatus(`Forbindelsen ble brutt: ${error.message}`, "feil");
  } else {
    view.showStatus("Frakoblet.", "frakoblet");
  }
}

const link = new SerialLink(handleLine, handleClosed);

document.getElementById("koble-til").addEventListener("click", async () => {
  try {
    await link.connect();
    view.setConnected(true);
    view.showStatus("Tilkoblet micro:bit. Venter på hendelser …", "tilkoblet");
  } catch (error) {
    view.showStatus(`Kunne ikke koble til: ${error.message}`, "feil");
  }
});

document.getElementById("koble-fra").addEventListener("click", () => link.disconnect());

if (!isWebSerialSupported()) {
  document.getElementById("koble-til").disabled = true;
  view.showStatus("Denne nettleseren støtter ikke WebSerial. Bruk Chrome eller Edge.", "feil");
}
