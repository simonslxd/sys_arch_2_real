// Connects to the micro:bit over WebSerial, delivers each complete text line, and can send text
// back (the status request).
// WebSerial works in Chrome and Edge, and only on https or http://localhost.

const MICROBIT_USB_VENDOR_ID = 0x0d28;   // the micro:bit's USB interface chip (DAPLink)
const BAUD_RATE = 115200;               // spec/serial-protocol.md

export function isWebSerialSupported() {
  return typeof navigator !== "undefined" && "serial" in navigator;
}

// Serial data arrives in chunks that can end in the middle of a line. LineBuffer collects the
// chunks and hands out complete lines, without "\r\n" and without empty lines.
export class LineBuffer {
  constructor() {
    this.pending = "";
  }

  push(chunk) {
    this.pending += chunk;
    const lines = this.pending.split("\n");
    this.pending = lines.pop();   // the part after the last "\n" is not a complete line yet
    return lines.map((line) => line.replace(/\r$/, "")).filter((line) => line !== "");
  }
}

export class SerialLink {
  // onLine(line) is called for every complete line; onClosed(error) when the connection ends,
  // with the error if it ended because something went wrong (e.g. the cable was pulled out).
  constructor(onLine, onClosed) {
    this.onLine = onLine;
    this.onClosed = onClosed;
    this.port = null;
    this.reader = null;
    this.readLoop = null;
  }

  // Asks the user to choose the micro:bit, then starts reading. Must be called from a click.
  async connect() {
    this.port = await navigator.serial.requestPort({ filters: [{ usbVendorId: MICROBIT_USB_VENDOR_ID }] });
    await this.port.open({ baudRate: BAUD_RATE });
    this.readLoop = this.readLines();
  }

  // Sends text to the micro:bit, e.g. the status request "STATUS\n" (spec/serial-protocol.md).
  async send(text) {
    const writer = this.port.writable.getWriter();
    try {
      await writer.write(new TextEncoder().encode(text));
    } finally {
      writer.releaseLock();
    }
  }

  async disconnect() {
    if (this.reader) {
      await this.reader.cancel();   // makes readLines() finish; it then closes the port
    }
    await this.readLoop;
  }

  async readLines() {
    const decoder = new TextDecoderStream();   // UTF-8, so "UTLÅNT" arrives intact
    const piping = this.port.readable.pipeTo(decoder.writable).catch(() => {});
    this.reader = decoder.readable.getReader();
    const buffer = new LineBuffer();
    let failure = null;
    try {
      while (true) {
        const { value, done } = await this.reader.read();
        if (done) {
          break;
        }
        buffer.push(value).forEach((line) => this.onLine(line));
      }
    } catch (error) {
      failure = error;
    } finally {
      this.reader.releaseLock();
      await piping;
      await this.port.close();
      this.reader = null;
      this.onClosed(failure);
    }
  }
}
