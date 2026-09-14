"""
Default Splitgate mitmproxy addon.

Redirects the game's backend hosts to the private server and passes other TLS
connections through untouched, so cert-pinned / unrelated hosts keep working.

The launcher supplies the host map via the SPLITGATE_REDIRECTS environment
variable as JSON: { "<original host>": "<target host>[:port]" }. https is
downgraded to http on redirect (the private server is plain http).
"""

import collections
import json
import os

from mitmproxy import http


# Used when the launcher didn't pass a map (e.g. an older build, or the env var got lost), so
# the proxy still redirects instead of silently passing everything through to the real backend.
DEFAULT_REDIRECTS = {"splitgate.accelbyte.io": "127.0.0.1:5005"}


def _load_redirects():
    try:
        parsed = json.loads(os.environ.get("SPLITGATE_REDIRECTS", "{}"))
        return parsed or DEFAULT_REDIRECTS
    except (ValueError, TypeError):
        return DEFAULT_REDIRECTS


REDIRECTS = _load_redirects()
print(f"[splitgate] redirecting: {REDIRECTS}")


def _split_target(value):
    """"host:port" -> ("host", port); "host" -> ("host", 80)."""
    host, sep, port = value.rpartition(":")
    if not sep:
        return value, 80
    return host, int(port)


class Proxy:
    def request(self, flow: http.HTTPFlow) -> None:
        target = REDIRECTS.get(flow.request.host)
        if target:
            host, port = _split_target(target)
            flow.request.scheme = "http"
            flow.request.host = host
            flow.request.port = port


class MaybeTls:
    """Conservative TLS passthrough: stop intercepting a host once its handshake fails."""

    def __init__(self):
        self.history = collections.defaultdict(lambda: collections.deque(maxlen=200))

    def tls_clienthello(self, data):
        address = data.context.server.peername
        if False in self.history[address]:
            data.ignore_connection = True

    def tls_established_client(self, data):
        self.history[data.context.server.peername].append(True)

    def tls_failed_client(self, data):
        self.history[data.context.server.peername].append(False)


addons = [Proxy(), MaybeTls()]
