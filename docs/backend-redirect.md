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

## Why it plausibly maps to Splitgate

Splitgate is an **Unreal Engine 4** title, like Fortnite, so the request path is the same shape:

- **UE HTTP** — the engine's `FHttpModule` / `IHttpRequest` builds requests to the game's
  backend (login, profile, matchmaking, etc.) and hands them to a platform HTTP implementation
  (on Windows, WinHTTP or libcurl).
- **Online subsystem** — Splitgate ships on Steam and uses an online backend (Steam and/or EOS
  for some services); those SDKs make their own calls.

The redirect sits at the moment a request's **URL/host is set**: when the host matches a
configured "official" host, swap it for the configured private host. Same idea as Sinum's
`Core/EOS` + `Core/Unreal` split — you'd cover whichever of {UE HTTP, EOS, Steam} Splitgate
actually uses.

**We already have the hard infrastructure for the client side:**

- DLL injection and function hooking (`Hook`, MinHook + vtable swaps — see
  [hooking.md](hooking.md)).
- A settings system with JSON persistence ([settings.md](settings.md)) — the natural home for a
  configurable target.
- The UE SDK (`Internal/ue/`) and the [Dumpspace dump](game-dump.md) to locate the HTTP/online
  classes and their request methods.

## Proposed shape (client side)

Config lives in the settings JSON — e.g. a new section:

```cpp
struct NetworkSettings {
    bool RedirectEnabled = false;                       // off by default
    // official host -> your private host
    std::map<std::string, std::string> HostRedirects;   // e.g. {"api.splitgate...": "api.myserver.dev"}
};
```

A feature (call it `BackendRedirect`) would, when `RedirectEnabled`, hook the request-URL entry
point(s) and, for any request whose host is a key in `HostRedirects`, rewrite it to the mapped
value before the request goes out. It fits the existing `Feature` framework and the
`SettingsChanged` event (toggle on/off live). The **matching/rewrite is host-only**; paths,
headers, and bodies are untouched, so the private backend implements the same API surface.

## What this study does NOT solve

- **The server.** Redirection is worthless without a **backend that speaks Splitgate's API**.
  That's the real work (login/auth, profile, matchmaking) and lives entirely on your server.
- **Exactly which endpoints/hosts Splitgate uses**, and via which stack (UE HTTP vs EOS vs
  Steam). This needs reverse engineering: capture your *own* client's traffic and read the SDK
  (the dump helps find the HTTP classes). Until that's known, the hook points are unknown.
- **TLS / certificate handling.** If the client validates or pins certificates for the official
  host, a rewritten host must still present something the client accepts (your own cert, or the
  request stack configured to trust it). This can be the deciding difficulty.
- **Anti-cheat / online-integrity systems**, which may detect a redirected client. Out of scope
  and a reason to keep this to private/offline use only.

## Verdict

Architecturally it's the **same technique as Sinum**, and the client side reuses infrastructure
this project already has (inject → hook → settings-driven toggle). The redirect itself
(host rewrite at the request-build site, target from settings JSON) is a modest feature. The
effort and the unknowns are almost entirely off the client: **identifying Splitgate's real
endpoints/stack**, **standing up a compatible backend**, and **TLS**. So: *feasible to wire the
client redirect; the project's a private-server backend, not a client hook.*

Recommended next step if pursued: a reverse-engineering pass (own-client traffic capture + dump
review) to enumerate the actual hosts and the stack that emits them, before writing any hook.
