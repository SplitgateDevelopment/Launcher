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
- Optional extra safety: if you ever see a crash *before* the worker (i.e. in `Init()` itself on
  the UI thread), wrap that `Globals::Init()` the same way; not needed for the reported crash.
