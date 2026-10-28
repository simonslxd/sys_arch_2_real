#!/usr/bin/env python3
"""Serves the dashboard on http://localhost:8000, without browser caching.

    python3 dashboard/serve.py        (in WSL; Ctrl+C stops it)

WebSerial needs localhost or https, and ES modules don't load from file://. The plain
`python3 -m http.server` lets the browser keep old copies of the JS modules, so after a change a
normal reload (F5) can mix new and old files, and the page stops working. This server tells the
browser not to keep copies (decision D-14).
"""
import functools
import http.server
from pathlib import Path

PORT = 8000


class NoCacheHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cache-Control", "no-store")
        super().end_headers()


if __name__ == "__main__":
    handler = functools.partial(NoCacheHandler, directory=str(Path(__file__).resolve().parent))
    with http.server.ThreadingHTTPServer(("127.0.0.1", PORT), handler) as server:
        print(f"Dashboard: http://localhost:{PORT}  (Ctrl+C stops it)", flush=True)
        server.serve_forever()
