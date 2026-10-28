// Keeps the events in a CSV file on the PC, using the browser's File System Access API (Chrome
// and Edge only). The user chooses the file once per browser session (assumption A-07).
// The file's content is described in csv_format.js.
import { CSV_HEADER, parseCsv } from "./csv_format.js";

const FILE_TYPES = [{ description: "CSV-fil", accept: { "text/csv": [".csv"] } }];

export function isFileAccessSupported() {
  return typeof window !== "undefined" && "showSaveFilePicker" in window;
}

export class CsvStore {
  constructor() {
    this.file = null;                  // the chosen file (a FileSystemFileHandle)
    this.queue = Promise.resolve();    // appends run one after another, in the order received
  }

  get fileName() {
    return this.file ? this.file.name : null;
  }

  // Asks for a new file and writes the header line into it. Must be called from a click.
  async createNew() {
    const file = await window.showSaveFilePicker({ suggestedName: "utlaan-hendelser.csv", types: FILE_TYPES });
    await write(file, CSV_HEADER + "\n", false);
    this.file = file;
    return { rows: [], rejected: [] };
  }

  // Asks for an existing file and reads it: { rows, rejected } as in parseCsv(). Throws if the file
  // is not an event file or may not be written to. Must be called from a click.
  async openExisting() {
    const [file] = await window.showOpenFilePicker({ types: FILE_TYPES });
    if ((await file.requestPermission({ mode: "readwrite" })) !== "granted") {
      throw new Error("fikk ikke lov til å skrive i filen");
    }
    const text = await (await file.getFile()).text();
    const content = parseCsv(text);
    if (text.trim() === "") {
      await write(file, CSV_HEADER + "\n", false);   // an empty file gets its header first
    }
    this.file = file;
    return content;
  }

  // Adds one line at the end of the file. Returns a promise that fails if this write fails;
  // later appends still run.
  append(row) {
    const result = this.queue.then(() => write(this.file, row, true));
    this.queue = result.catch(() => {});
    return result;
  }
}

async function write(file, text, atTheEnd) {
  const writable = await file.createWritable({ keepExistingData: atTheEnd });
  if (atTheEnd) {
    await writable.seek((await file.getFile()).size);
  }
  await writable.write(text);
  await writable.close();
}
