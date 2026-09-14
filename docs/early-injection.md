# Early injection — covering pre-injection network calls

**Problem.** The backend redirect ([backend-redirect.md](backend-redirect.md)) hooks the game's
HTTP in-process, but the DLL is only injected *after* the game has started — today via the
`WH_GETMESSAGE` window hook, which fires once the game has a window and message loop. By then
the game may already have made its **early backend calls** (notably the initial login/auth
against `splitgate.accelbyte.io`). Those calls go to the real server, not the private one,
because our hook wasn't installed yet.

The external Fiddler/mitmproxy setup doesn't have this problem: it's a system-wide proxy that's
already running before the game launches, so it catches the very first packet. Our goal is an
in-process solution that closes the same gap without an external proxy.

This is a **design note** — the recommended fix lives in the launcher and is **not
implemented** here.

## What we need

The hooks (`curl_easy_setopt` / WinHTTP) must be **installed before the game runs any code that
issues a backend request**. Everything else already works — redirect logic, the settings map,
logging. The only missing property is *ordering*: hook first, then let the game run.

## Options

### 1. Launch suspended → inject → resume (recommended)

The launcher already starts/owns the game process. Instead of letting it run freely and
injecting via the window hook, the launcher:

1. `CreateProcess(..., CREATE_SUSPENDED, ...)` — the game's main thread is created **frozen**,
   before a single instruction of game code executes.
2. Inject the DLL while suspended — write the DLL path into the target and run `LoadLibrary`
   there via a remote thread (classic `CreateRemoteThread` + `LoadLibraryW`), *or* keep the
   current `SetWindowsHookEx` mechanism but installed before resume. The DLL's `DllMain` /
   init installs the network hooks immediately.
3. `ResumeThread` — the game starts with our hooks **already live**, so no request escapes.

Why it's the right answer:

- **Deterministic.** Hooks precede all game code; the login call is covered.
- **Self-contained.** No external proxy, no OS-wide changes, no certificates — the in-process
  `http://` downgrade still handles TLS the same way.
- **Reuses what we have.** The redirect/hook code is unchanged; only the launcher's start +
  inject sequence changes.

Costs / caveats:

- The injection method changes from the `WH_GETMESSAGE` window hook (which needs a window, i.e.
  a running game) to **early injection at process creation**. That's a launcher change (see
  [hooking.md](hooking.md) for the current flow) — the DLL side installs hooks in its init as
  it does now.
- `DllMain` runs under loader lock, so the network hooks (MinHook + a `LoadLibrary("winhttp")`)
  should be installed from a short deferred step or the existing `Hook::Init`, not directly in
  `DllMain`, to avoid loader-lock deadlocks. The DLL already defers heavy init out of the entry
  point, so this fits.
- Anti-tamper/anti-debug in the game may react to a suspended-launch injector; a modding-only
  concern, but worth knowing.

### 2. Launcher sets a temporary OS-level redirect

Before launch, the launcher adds a `hosts` entry (`splitgate.accelbyte.io → 127.0.0.1`) or a
temporary system proxy pointing at the private server, then removes it on exit. This catches
the first call, but:

- It's essentially **re-implementing the external proxy** the in-process hook was meant to
  replace.
- A `hosts` redirect keeps the original **port 443 and TLS**, so the private server would need
  to serve HTTPS on 443 with a certificate the client trusts — reintroducing the exact TLS
  problem the in-process `https→http` downgrade avoids.
- Requires admin rights (editing `hosts` / system proxy) and leaves state to clean up.

Not recommended — strictly worse than option 1 for our setup.

### 3. Accept the gap

If the pre-injection calls are non-critical or the game **retries** them after a failure (many
online clients retry auth), the post-injection hook may suffice in practice. Cheapest (nothing
to build), but unreliable for login — the one call that most needs redirecting is the earliest.

## Recommendation

**Option 1 — suspended launch + early injection, in the launcher.** It's deterministic,
self-contained, and reuses the existing hook code; only the launcher's process-start sequence
changes. It's out of scope for the network module itself and is left unimplemented until the
launcher work is picked up.
