// Drives the real dashboard (http://localhost:8000) in headless Chrome over the DevTools protocol,
// with stubs.js in place of the serial port and the file picker.
//
//   python3 dashboard/serve.py                (first, in another terminal)
//   node tests/browser/run.mjs <chrome or msedge> spec/serial-protocol-examples.txt <output dir>
//
// Phase 1: new CSV file, connect, the micro:bit sends golden lines 1-12 (the actions).
// Phase 2: reload the page, open the same file: the stored rows come back as source "fil".
// Writes result.json, the CSV file and screenshots (desktop and phone width) to <output dir>.
import { spawn } from "node:child_process";
import { readFileSync, writeFileSync, mkdirSync } from "node:fs";
import { join } from "node:path";
import { tmpdir } from "node:os";

const [chrome, goldenFile, outDir] = process.argv.slice(2);
mkdirSync(outDir, { recursive: true });
const stubs = readFileSync(new URL("./stubs.js", import.meta.url), "utf8");
const actions = readFileSync(goldenFile, "utf8").split("\n").map((l) => l.replace(/\r$/, ""))
  .filter((l) => l && !l.startsWith("#"))
  .filter((l) => !/,(START|STATUS),/.test(l))
  .map((l) => l.slice(l.indexOf(",") + 1));          // the payload without seq

const PORT = 9333;
const browser = spawn(chrome, ["--headless=new", `--remote-debugging-port=${PORT}`,
  `--user-data-dir=${join(tmpdir(), "dashboard-browser-check")}`, "--no-first-run", "about:blank"], { stdio: "ignore" });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

let target;
for (let i = 0; i < 50 && !target; i++) {
  await sleep(200);
  try {
    target = (await (await fetch(`http://127.0.0.1:${PORT}/json/list`)).json()).find((t) => t.type === "page");
  } catch { /* not up yet */ }
}
const ws = new WebSocket(target.webSocketDebuggerUrl);
await new Promise((r) => ws.addEventListener("open", r));
let nextId = 1;
const pending = new Map();
const listeners = [];
ws.addEventListener("message", (msg) => {
  const data = JSON.parse(msg.data);
  if (data.id && pending.has(data.id)) {
    pending.get(data.id)(data);
    pending.delete(data.id);
  } else if (data.method) {
    listeners.forEach((l) => l(data));
  }
});
const cdp = (method, params = {}) => new Promise((resolve, reject) => {
  const id = nextId++;
  pending.set(id, (d) => (d.error ? reject(new Error(`${method}: ${d.error.message}`)) : resolve(d.result)));
  ws.send(JSON.stringify({ id, method, params }));
});
const evaluate = async (expression) => {
  const r = await cdp("Runtime.evaluate", { expression, awaitPromise: true, returnByValue: true });
  if (r.exceptionDetails) throw new Error(r.exceptionDetails.exception?.description ?? expression);
  return r.result.value;
};
const loaded = () => new Promise((r) => listeners.push((d) => d.method === "Page.loadEventFired" && r()));
const errors = [];
listeners.push((d) => {
  if (d.method === "Runtime.exceptionThrown") errors.push(d.params.exceptionDetails.exception?.description);
  if (d.method === "Runtime.consoleAPICalled" && d.params.type === "error") errors.push(d.params.args.map((a) => a.value).join(" "));
});

async function screenshot(name, width) {
  await cdp("Emulation.setDeviceMetricsOverride", { width, height: 900, deviceScaleFactor: 1, mobile: width < 600 });
  await sleep(300);
  const { cssContentSize } = await cdp("Page.getLayoutMetrics");
  const shot = await cdp("Page.captureScreenshot", { format: "png", captureBeyondViewport: true,
    clip: { x: 0, y: 0, width, height: Math.ceil(cssContentSize.height), scale: 1 } });
  writeFileSync(join(outDir, name), Buffer.from(shot.data, "base64"));
  return { scrollWidth: await evaluate("document.documentElement.scrollWidth"), width };
}

const table = (id) => `[...document.querySelectorAll('#${id} tbody tr')].map((r) => [...r.cells].map((c) => c.textContent))`;
const rect = (sel) => `(() => { const r = document.querySelector('${sel}').getBoundingClientRect(); return { x: Math.round(r.x), y: Math.round(r.y), w: Math.round(r.width), h: Math.round(r.height) }; })()`;

await cdp("Page.enable");
await cdp("Runtime.enable");
await cdp("Page.addScriptToEvaluateOnNewDocument", { source: stubs });
await cdp("Emulation.setDeviceMetricsOverride", { width: 1280, height: 900, deviceScaleFactor: 1, mobile: false });

// Phase 1
let wait = loaded();
await cdp("Page.navigate", { url: "http://localhost:8000/" });
await wait;
await sleep(300);
const result = { before: {} };
result.before.storageStatus = await evaluate("document.getElementById('lagring-status').textContent");
await evaluate("document.getElementById('ny-fil').click()");
await sleep(300);
await evaluate("document.getElementById('koble-til').click()");
await sleep(500);
for (const payload of actions) {
  await evaluate(`window.__microbit.send(${JSON.stringify(payload)})`);
  await sleep(60);
}
await sleep(500);
result.phase1 = {
  sentToMicrobit: await evaluate("window.__microbit.writes"),
  csv: await evaluate("sessionStorage.getItem('fakeCsv')"),
  events: await evaluate(table("hendelser")),
  items: await evaluate(table("utstyrsstatus")),
  selection: await evaluate("[document.getElementById('valgt-student').textContent, document.getElementById('valgt-utstyr').textContent]"),
  headings: await evaluate("[...document.querySelectorAll('h2, dt, th')].map((e) => e.textContent)"),
  layout: {
    lagring: await evaluate(rect(".kontroll > section:nth-child(1)")),
    tilkobling: await evaluate(rect(".kontroll > section:nth-child(2)")),
    valgt: await evaluate(rect(".oversikt > section:nth-child(1)")),
    utstyrsstatus: await evaluate(rect(".oversikt > section:nth-child(2)")),
    hendelser: await evaluate(rect("#hendelser")),
  },
  storageStatus: await evaluate("document.getElementById('lagring-status').textContent"),
  rejected: await evaluate("[...document.querySelectorAll('#avviste li')].map((li) => li.textContent)"),
};
writeFileSync(join(outDir, "utlaan-hendelser.csv"), result.phase1.csv);
result.phase1.desktop = await screenshot("dashboard-desktop.png", 1280);
result.phase1.phone = await screenshot("dashboard-phone.png", 390);
result.phase1.phoneLayout = {
  lagring: await evaluate(rect(".kontroll > section:nth-child(1)")),
  tilkobling: await evaluate(rect(".kontroll > section:nth-child(2)")),
};
await cdp("Emulation.setDeviceMetricsOverride", { width: 1280, height: 900, deviceScaleFactor: 1, mobile: false });

// Phase 2: reload and reopen the file
wait = loaded();
await cdp("Page.reload", { ignoreCache: true });
await wait;
await sleep(300);
await evaluate("document.getElementById('apne-fil').click()");
await sleep(500);
result.phase2 = {
  events: await evaluate(table("hendelser")),
  storageStatus: await evaluate("document.getElementById('lagring-status').textContent"),
};
result.errors = errors;

// The checks. Golden lines 1-12 hold 6 loans/returns: 4 OK and 2 FEIL (spec/serial-protocol-examples.txt).
const csvRows = result.phase1.csv.trim().split("\n").slice(1).map((line) => line.split(","));
const p1 = result.phase1;
const sameRow = (a, b) => a.y === b.y && a.h > 0 && b.h > 0;
const checks = {
  "CSV: exactly the 4 successful loans/returns are stored (REQ-STO-01)":
    csvRows.length === 4 && csvRows.every((row) => row[6] === "OK"),
  "Page: only the 4 successful loans/returns are listed live, no FEIL (REQ-UI-01)":
    p1.events.length === 4 && p1.events.every((row) => row[8] === "OK"),
  "Reload + open file: the 4 stored rows come back as source 'fil' (REQ-STO-02)":
    result.phase2.events.length === 4 && result.phase2.events.every((row) => row[1] === "fil"),
  "Layout: Lagring and Tilkobling side by side (desktop)": sameRow(p1.layout.lagring, p1.layout.tilkobling),
  "Layout: Valgt på stasjonen and Utstyrsstatus side by side (desktop)": sameRow(p1.layout.valgt, p1.layout.utstyrsstatus),
  "Layout: stacked and without horizontal scrolling at 390 px":
    p1.phone.scrollWidth <= 390 && p1.phoneLayout.tilkobling.y > p1.phoneLayout.lagring.y,
  "Text: no code names (studentnummer, utstyr_navn, Lab_Utstyr) in headings or labels":
    !p1.headings.some((text) => /studentnummer|utstyr_navn|Lab_Utstyr|\(/.test(text.replace("(CSV-fil)", "").replace("(nyeste først)", ""))),
  "No JS errors on the page": errors.length === 0,
};
result.checks = checks;
writeFileSync(join(outDir, "result.json"), JSON.stringify(result, null, 2));
ws.close();
browser.kill();
for (const [name, ok] of Object.entries(checks)) console.log(`${ok ? "PASS" : "FAIL"}  ${name}`);
const failed = Object.values(checks).filter((ok) => !ok).length;
console.log(failed ? `${failed} check(s) failed` : "all checks passed");
process.exit(failed ? 1 : 0);
