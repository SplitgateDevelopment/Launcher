# TODO — changes needed in forbidden files

These are in files I can't edit (`Internal/dllmain.cpp`, `Launcher/Launcher.cpp`,
`Internal/hook/Hook.h`).

## 1. Fix the injection trigger-message id (THIS is why the DLL never initializes)

The launcher posts the trigger message resolved with `RegisterWindowMessageW(L"SplitgateInit")`,
but `dllmain.cpp` still checks the message against `WM_APP + 1`. The two ids don't match, so
`SplitgateCallBack` drops the trigger, `Hook::Init()` never runs (no console, no `internal.log`),
and the launcher reports **"DLL failed to initialize (timed out)"**.

In `Internal/dllmain.cpp`, resolve the id the same way the launcher does — `RegisterWindowMessageW`
returns the identical value in every process for the same string:

```cpp
// before
constexpr UINT WM_SPLITGATE_INIT = WM_APP + 1;

// after — it's a runtime call now, so not constexpr; compute it once
static const UINT WM_SPLITGATE_INIT = RegisterWindowMessageW(L"SplitgateInit");
```

Leave the `if (msg->message != WM_SPLITGATE_INIT) ...` check unchanged. (This matches the launcher
and [docs/hooking.md](docs/hooking.md#trigger-message-id).)

## 2. Pass the game PID (and logger) to the mitmproxy spawn — optional

`Launcher::Mitmproxy::Spawn(redirects, gamePid = 0, logger = nullptr)`. `Spawn` already resolves
the game PID itself when passed 0, so this is optional — pass `&logger` if you want it to log
"proxy started / scripts not found / mitmdump not on PATH":

```cpp
Launcher::Mitmproxy::Spawn(network.Redirects, gamePid, &logger);
```

## 3. Fix the early-injection crash in `Hook::InitWorld` (`Internal/hook/Hook.h`)

**Symptom.** Injecting once a match/world is already loaded works; injecting *while the game just
started* crashes the game.

**Cause.** `InitWorld` (on the detached worker thread) walks
`World -> OwningGameInstance -> LocalPlayers[0] -> ViewportClient -> VFTable` with raw
dereferences inside its retry loop. During early injection those pointers are
partially-constructed, so the dereference access-violates and takes the game down instead of
retrying. The `Globals::Init()` on the UI thread in `Init()` is safe (it only reads globals that
may be null — `EngineInit()` already guaranteed `GObjects` is valid before it runs), so the fix is
confined to the worker loop.

**Fix.** You already added the SEH-guarded `TryResolveViewport(...)` next to `InitWorld`, but the
loop still uses the raw walk. Route the loop through it — an access violation becomes a `nullptr`
return + retry rather than a crash. Replace the loop body in `InitWorld`:

```cpp
// before
for (int attempt = 0; attempt < 600; ++attempt)
{
    Globals::Init(); // re-resolve the world (it changes across map loads)

    UGameInstance* OwningGameInstance = Globals::World ? Globals::World->OwningGameInstance : nullptr;
    if (OwningGameInstance)
    {
        TArray<ULocalPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;
        if (LocalPlayers.Num() > 0 && LocalPlayers[0]) // guard against an empty array (OOB read)
        {
            LocalPlayer = (UPortalWarsLocalPlayer*)LocalPlayers[0];
            ViewPortClient = LocalPlayer->ViewportClient;
            ViewPortClientVTable = ViewPortClient ? ViewPortClient->VFTable : nullptr;
            if (ViewPortClientVTable) break;
        }
    }

    LocalPlayer = nullptr;
    ViewPortClient = nullptr;
    ViewPortClientVTable = nullptr;

    if (attempt == 0)
    {
        Logger::Log("INFO", "Waiting for the world / local player to be ready...");
    }

    Sleep(100);
}

// after — the whole walk is now behind the __try in TryResolveViewport
for (int attempt = 0; attempt < 600; ++attempt)
{
    ViewPortClientVTable = TryResolveViewport(LocalPlayer);
    if (ViewPortClientVTable)
    {
        ViewPortClient = LocalPlayer->ViewportClient; // LocalPlayer + its ViewportClient already validated
        break;
    }

    LocalPlayer = nullptr;
    ViewPortClient = nullptr;

    if (attempt == 0)
        Logger::Log("INFO", "Waiting for the world / local player to be ready...");

    Sleep(100);
}
```

Notes:
- `TryResolveViewport` must sit in a helper with **no unwinding objects** in the `__try` scope
  (the `TArray` copy is fine — it has no destructor); yours already satisfies this, so keep the
  raw walk *inside* the `__try`, not in `InitWorld`.
- Optional cleanup: give `TryResolveViewport` a second out-param
  (`UGameViewportClient*& outViewport`) and set `ViewPortClient` from it, so the post-loop
  `LocalPlayer->ViewportClient` re-read disappears entirely. Not required — the re-read is safe
  because the guard already confirmed both are non-null. The two-out-param version:

  ```cpp
  // Resolve World -> OwningGameInstance -> LocalPlayers[0] -> ViewportClient (+ its VFTable)
  // under SEH so partially-constructed pointers during early injection yield nullptr instead of
  // an access violation. Keep this a leaf helper with no unwinding objects in the __try scope
  // (the TArray copy has no destructor, so it's allowed).
  inline void** TryResolveViewport(UPortalWarsLocalPlayer*& outLocalPlayer, UGameViewportClient*& outViewport)
  {
      __try
      {
          Globals::Init(); // re-resolve the world (it changes across map loads)

          UGameInstance* gameInstance = Globals::World ? Globals::World->OwningGameInstance : nullptr;
          if (!gameInstance) return nullptr;

          TArray<ULocalPlayer*> localPlayers = gameInstance->LocalPlayers;
          if (localPlayers.Num() <= 0 || !localPlayers[0]) return nullptr;

          auto* localPlayer = reinterpret_cast<UPortalWarsLocalPlayer*>(localPlayers[0]);
          auto* viewport = localPlayer->ViewportClient;
          if (!viewport || !viewport->VFTable) return nullptr;

          outLocalPlayer = localPlayer;
          outViewport = viewport;
          return viewport->VFTable;
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
          return nullptr;
      }
  }
  ```

  with the loop becoming:

  ```cpp
  for (int attempt = 0; attempt < 600; ++attempt)
  {
      ViewPortClientVTable = TryResolveViewport(LocalPlayer, ViewPortClient);
      if (ViewPortClientVTable) break;

      LocalPlayer = nullptr;
      ViewPortClient = nullptr;

      if (attempt == 0)
          Logger::Log("INFO", "Waiting for the world / local player to be ready...");

      Sleep(100);
  }
  ```
**Optional extra safety — guarding `Init()`'s own `Globals::Init()` (explained).**

`Hook::Init()` runs on the game's **UI/message thread** (it's called when the `WH_GETMESSAGE`
trigger is pulled from the queue). It does `EngineInit()` → `Globals::Init()` → spawn the worker.
That first `Globals::Init()` is normally safe, and here's *why* it's separate from the worker crash:

- `Globals::Init()` only does **single-level reads** of engine globals: `UEngine::GetEngine()`
  (reads `GEngine`), `UWorld::GetWorld()` (reads `GWorld`), and a few `StaticClass()` CDO lookups
  (`FindObject`). If `GEngine`/`GWorld` aren't set yet they read back **null** — no chained
  dereference, so no fault. And `FindObject` is safe once `EngineInit()` has **validated the
  `GObjects`/`GNames` offsets** (it returns false and `Init()` bails if they weren't found), so the
  object-array walk never runs over garbage.
- The crash you hit was different: the **worker's** `World -> OwningGameInstance ->
  LocalPlayers[0] -> ViewportClient -> VFTable` is a *chain* of dereferences, so a
  partially-constructed link mid-chain access-violates. That's what `TryResolveViewport`'s SEH now
  absorbs.

So `Init()`'s `Globals::Init()` was **not** the culprit and needs no change today. The only way it
could fault is if you inject so early that a global is non-null but *mid-construction* (e.g. `GWorld`
points at a `UWorld` whose fields are still being built) and one of those reads touches unmapped
memory. **If** you ever see a crash whose stack sits in `Init()` / `Globals::Init()` on the UI thread
(not the worker), the fix is the same SEH shape — but you **can't** `Sleep`-retry on the UI thread
(that freezes the game). Instead, make the UI-thread `Globals::Init()` best-effort: wrap it in a
leaf `__try/__except` helper (no unwinding objects in scope, like `TryResolveViewport`) that swallows
the fault and returns, and rely on the worker's `TryResolveViewport` (which calls `Globals::Init()`
again, guarded, and *can* retry) to do the real resolve. In other words, the eager UI-thread
`Globals::Init()` is just a convenience — it's safe to guard or even drop, because the worker
re-resolves everything anyway.

Code, if you ever need it. Add a leaf SEH wrapper next to `TryResolveViewport` (same rule: no
objects needing unwinding in the `__try` scope):

```cpp
// Best-effort Globals::Init() for the UI thread: an early-injection fault while a global is
// mid-construction returns false instead of crashing. The worker (TryResolveViewport) resolves
// for real and can retry, so a false here is harmless. No unwinding objects in the __try scope.
inline bool TryGlobalsInit()
{
    __try
    {
        Globals::Init();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
```

Then in `Init()`, swap the eager call:

```cpp
// before
Globals::Init();

// after — don't take down the UI thread if globals aren't safe to read this early
TryGlobalsInit(); // best-effort; the worker's TryResolveViewport does the real (retryable) resolve
```

If you ever see the fault in `EngineInit()` itself (the offset/signature scan) rather than
`Globals::Init()`, wrap that call the same way (`if (!TryEngineInit()) return FALSE;`) — but that's
even less likely, since `EngineInit()` already validates what it resolves.
