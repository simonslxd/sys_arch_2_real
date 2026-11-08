// Injected before the dashboard's own scripts. Replaces the two browser features that need a
// person (choosing a serial port, choosing a file) with fakes:
//  - the CSV file lives in sessionStorage, so it survives a reload of the page
//  - the micro:bit is a fake port; window.__microbit.send(payload) sends "<seq>,<payload>\r\n",
//    with seq counting up like the firmware's, and the STATUS request is answered as the
//    firmware does (one STATUS line per item, at the start of the golden-line session)
(() => {
  const KEY = "fakeCsv";
  const read = () => sessionStorage.getItem(KEY) ?? "";
  const handle = {
    name: "utlaan-hendelser.csv",
    async getFile() {
      const text = read();
      return { size: new TextEncoder().encode(text).length, text: async () => text };
    },
    async requestPermission() { return "granted"; },
    async createWritable({ keepExistingData } = {}) {
      let content = keepExistingData ? read() : "";
      return {
        async seek() {},                       // the dashboard seeks to the end: we append
        async write(text) { content += text; },
        async close() { sessionStorage.setItem(KEY, content); },
      };
    },
  };
  window.showSaveFilePicker = async () => { sessionStorage.setItem(KEY, ""); return handle; };
  window.showOpenFilePicker = async () => [handle];

  const encoder = new TextEncoder();
  const writes = [];
  let controller = null;
  let seq = 0;
  const send = (payload) => controller.enqueue(encoder.encode(`${seq++},${payload}\r\n`));
  const STATUS_ANSWER = [
    "STATUS,S001,Multimeter,LEDIG,OK,2,2",
    "STATUS,S001,Oscilloskop,LEDIG,OK,1,1",
    "STATUS,S001,Loddestasjon,LEDIG,OK,3,3",
  ];
  const port = {
    async open() {
      this.readable = new ReadableStream({ start(c) { controller = c; } });
      this.writable = new WritableStream({
        write(chunk) {
          const text = new TextDecoder().decode(chunk);
          writes.push(text);
          if (text === "STATUS\n") STATUS_ANSWER.forEach(send);
        },
      });
    },
    async close() {},
  };
  Object.defineProperty(navigator, "serial", {
    configurable: true,
    value: { requestPort: async () => port },
  });
  window.__microbit = { send, writes };
})();
