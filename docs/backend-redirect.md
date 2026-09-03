# Backend redirection — feasibility study

Can we do what [Sinum](https://github.com/projectnovafn/Sinum) does — point the game at a
self-hosted backend instead of the official one — with the target **configurable via the
settings JSON**? Yes; a first cut is implemented (see [Implementation](#implementation-first-cut)
below). This note covers the architecture and the open questions.

> [!IMPORTANT]
> **Scope.** This is only meaningful for a **private server you operate**, against **your own
> game client**, for modding/preservation/testing. Redirecting a live commercial game's
> traffic can violate its Terms of Service, and pointing a client at a server means that
> server sees whatever the client sends it — so **never** use this to intercept other players'
> traffic or credentials, and never distribute a build that redirects someone else's client to
> a server they didn't choose. Treat the official servers as off-limits; you only ever talk to
> your own.

## What Sinum does (from its README + layout)

Sinum is a **request-redirection** layer: it *"redirect[s] requests from a target URL, such as
`ol.epicgames.com`, to another server, like `api.novafn.dev`."* On Windows it's a **DLL
injected** into the game; its `Core/` holds the redirection for two backends a UE title uses —
the **EOS SDK** (`Core/EOS.*`) and **Unreal's own HTTP** (`Core/Unreal/`). In other words: it
hooks the client's outbound-request path and rewrites the **host** from the official endpoint
to the private one, leaving the rest of the request intact so a compatible backend can answer.

That's the whole trick — it's not a network proxy, it's an in-process **host rewrite** at the
point the game builds its requests.

## How it maps to Splitgate

Splitgate's official backend is **AccelByte** — a single HTTPS host, **`splitgate.accelbyte.io`**
(namespace `splitgate`) — reached over UE's HTTP stack (`FHttpModule` on top of WinHTTP/libcurl),
**plus one WebSocket** (AccelByte's *lobby* service). It is *not* Epic/EOS; the stack is much
narrower than Fortnite's.

This is already known concretely because **the private server exists**: a Bun/Hono/`ws`
AccelByte emulator (org `@splitgatedevelopment`) that answers the AccelByte API on port
**5005** — `/iam/v3/*` (OAuth, platform=steam), `/basic/public/namespaces/splitgate/*`,
`/sessionmanager/namespaces/splitgate/*`, plus items/leaderboard/lobby/parties/profile/season-pass/
etc., and the lobby WebSocket. So the "compatible backend" half of the problem is solved.

**And redirection already works today — just externally.** The server is currently reached by a
system proxy: a **Fiddler auto-responder** (`regex:(.*)splitgate.accelbyte.io/(.*)` →
`http://localhost:5005/$2`) or **mitmproxy** (`mitmdump -k`, system proxy `127.0.0.1:8080`). The
DLL feature's whole job is to move that one host-rewrite **in-process**, so no external proxy /
system-wide setting is needed. The redirect is therefore a **single-host** rewrite
(`splitgate.accelbyte.io` → the configured server) plus the WebSocket host, not a survey of
multiple SDKs.

**We already have the hard infrastructure for the client side:**

- DLL injection and function hooking (`Hook`, MinHook + vtable swaps — see
  [hooking.md](hooking.md)).
- A settings system with JSON persistence ([settings.md](settings.md)) — the natural home for a
  configurable target.
- The UE SDK (`Internal/ue/`) and the [Dumpspace dump](game-dump.md) to locate the HTTP/online
  classes and their request methods.

## Proposed shape (client side)

Config lives in the settings JSON — e.g. a new section defaulted to the known host:

```cpp
struct NetworkSettings {
    bool RedirectEnabled = false;                          // off by default
    std::string OfficialHost = "splitgate.accelbyte.io";   // what to match
    std::string PrivateHost = "127.0.0.1:5005";            // where to send it
};
```

A feature (call it `BackendRedirect`) would, when `RedirectEnabled`, hook the request-URL entry
point and rewrite `OfficialHost` → `PrivateHost` before each request goes out — plus the
matching WebSocket URL for the lobby. It fits the existing `Feature` framework and the
`SettingsChanged` event (toggle on/off live). The **rewrite is host-only**; paths, headers, and
bodies are untouched, so the emulator answers the same AccelByte API surface — exactly what the
Fiddler/mitmproxy rule does today, just in-process.

## What this study does NOT solve

- **TLS / certificate handling — the real crux.** The official host is HTTPS, so a rewritten
  request must terminate TLS somewhere the client trusts. The current external setup sidesteps
  this with `mitmdump -k` / Fiddler's HTTPS decryption (a trusted root cert). Moving in-process
  has to answer the same question: point at `http://127.0.0.1:5005` and stop the client
  upgrading to TLS, or hook the request below TLS, or install/trust a local cert for the host.
  This is the deciding difficulty, not the routing.
- **The exact client hook point.** Where UE sets the request URL (or the WinHTTP/curl connect)
  still needs pinning down via RE — capture your *own* client's traffic and use the
  [dump](game-dump.md) to find the HTTP classes. (This is narrower now: one host + one WS.)
- **The WebSocket.** The lobby WS host must be redirected too, and it's a separate connect path
  from HTTP.
- **Anti-cheat / online-integrity**, which may notice a redirected client — a reason to keep
  this to private/offline use only.

Note the **server half is already done**: the AccelByte emulator exists and works behind the
external proxy, so this is purely about replacing that proxy with an in-process hook.

## Verdict

Architecturally it's the **same technique as Sinum**, the client side reuses infrastructure this
project already has (inject → hook → settings-driven toggle), and — unlike a from-scratch
private-server project — **both the backend and a working (external) redirect already exist**.
So the task reduces to a **single-host in-process rewrite** (`splitgate.accelbyte.io` → the
local server, plus the lobby WS), whose one hard part is **TLS termination** (matching what
`mitmdump -k` / Fiddler do today). *Feasible, and much smaller than the generic Sinum case.*

Recommended next step if pursued: a short RE pass on your own client (traffic capture + dump
review) to find the URL/connect hook point and confirm how TLS is handled, then wire the
settings-driven rewrite.

## Implementation

Lives in **`Internal/network/`** and is configured from the **Network** menu tab (backed by
the `NETWORK` settings section). Structure:

- **`Redirect.h`** — pure logic (no OS deps, unit-tested): `HostOf`, `Target` (map lookup),
  `RewriteUrl` (downgrades `https://<host>` → `http://<target>`), `IsRedirectHost` (log filter).
- **`HttpLogger.h`** — `Http::Log(method, url)` → console and optional `http.log`, with an
  optional "redirected hosts only" filter.
- **`CurlHook.h`** — the **libcurl** hook (the game's real stack; see below).
- **`WinHttpHook.h`** — the WinHTTP hook (offset-free fallback).
- **`Network.h`** — `Network::Init()` installs both hooks; called once from `Features::Init`.

**Config is a map**, not a single host: `Settings.NETWORK.Redirects` is
`{ "original host" → "host[:port]" }` (default `splitgate.accelbyte.io` → `127.0.0.1:5005`),
plus `RedirectEnabled` and the three `HttpLog*` flags. Everything self-gates on the settings,
so the Network tab toggles/edits behavior live. HTTPS is downgraded to HTTP on redirect (the
private server is plain HTTP, matching the Fiddler rule).

### Getting around libcurl

Splitgate's UE HTTP goes through **libcurl** (`FCurlHttpRequest`), which is **statically
linked**, so its symbols aren't exported and `GetProcAddress` can't find them. The approach
(`CurlHook.h`):

1. **Locate `curl_easy_setopt` by an AOB signature.** Since it isn't exported, we scan the game
   module for its prologue with `Util.h::FindSignature`. The signature is **build-specific and
   left empty** — fill it once from a RE pass (IDA/Ghidra/x64dbg); `0x00` bytes are wildcards.
   Until it's set, the curl hook is inert and WinHTTP is the fallback.
2. **Hook it and rewrite `CURLOPT_URL`.** `curl_easy_setopt(CURL*, CURLoption, ...)` is
   variadic, but on x64 the single vararg lands in one register, so a three-parameter prototype
   is ABI-compatible for the `CURLOPT_URL` (a `char*`) case. On that option we log the URL and,
   if its host is a redirect key, pass `RewriteUrl(url)` instead. curl copies the string during
   `setopt`, so a rewritten temporary is safe.

That single choke point covers **all** of the game's HTTP (login, profile, matchmaking, feed),
because every request's URL flows through `curl_easy_setopt`.

### How to verify

Enable the toggle in the Network tab with the console open. On a redirect you'll see
`[Network] curl https://splitgate.accelbyte.io/… -> http://127.0.0.1:5005/…` (or the WinHTTP
variant). With HTTP logging on, every call is printed (and optionally written to `http.log`, or
watched live in the Network tab's **Request flow** section) — which also confirms which stack
the game uses.

### Known limitations

- **libcurl signature.** The one piece needing your RE — an empty AOB until you fill it. WinHTTP
  works offset-free in the meantime.
- **Timing** — the in-process hook only covers calls made *after* injection, so the earliest
  backend call (login) can escape it. This has its own design note:
  [early-injection.md](early-injection.md) (recommended fix: suspended-launch + early injection
  in the launcher; not implemented).
- **Live map edits** race the network threads that read the map; fine for occasional edits
  (the map is normally set in the JSON before launch), but not lock-protected yet.
- Not built/tested against the game here (DLL needs the VS toolchain); the pure redirect logic,
  the HTTP-log buffer, and the settings round-trip are covered by the test suite
  (`Tests/NetworkTests.cpp`).
