from __future__ import annotations

import argparse
import json
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from typing import Any

from predict_ocr import predict_ocr


class OCRAPIHandler(BaseHTTPRequestHandler):
    server_version = "OCRLocalAPI/1.0"

    def _send_json(self, status: int, payload: Any) -> None:
        body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self) -> None:  # noqa: N802
        if self.path == "/health":
            self._send_json(200, {"status": "ok"})
            return
        self._send_json(404, {"error": "not found"})

    def do_POST(self) -> None:  # noqa: N802
        if self.path != "/predict_ocr":
            self._send_json(404, {"error": "not found"})
            return

        try:
            content_length = int(self.headers.get("Content-Length", "0"))
            raw = self.rfile.read(content_length)
            payload = json.loads(raw.decode("utf-8"))

            c = float(payload["c"])
            t = float(payload["T"])
            p = float(payload["P"])
        except KeyError as exc:
            self._send_json(400, {"error": f"missing field: {exc.args[0]}"})
            return
        except (ValueError, TypeError, json.JSONDecodeError):
            self._send_json(400, {"error": "invalid JSON payload, expected numeric c/T/P"})
            return

        result = predict_ocr(c=c, T=t, P=p)
        self._send_json(200, result)

    def log_message(self, format: str, *args: Any) -> None:  # noqa: A003
        # keep local API output clean
        return


def main() -> None:
    parser = argparse.ArgumentParser(description="Local OCR API")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    args = parser.parse_args()

    server = ThreadingHTTPServer((args.host, args.port), OCRAPIHandler)
    print(f"OCR local API listening on http://{args.host}:{args.port}")
    server.serve_forever()


if __name__ == "__main__":
    main()
