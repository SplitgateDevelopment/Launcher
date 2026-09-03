# Backend redirection — feasibility study

Can we do what [Sinum](https://github.com/projectnovafn/Sinum) does — point the game at a
self-hosted backend instead of the official one — with the target **configurable via the
settings JSON**? This is an architecture/feasibility note, not an implementation.

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
