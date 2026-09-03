# TODO — changes needed in forbidden files

These are in files I can't edit (`Internal/dllmain.cpp`, `Launcher/Launcher.cpp`).

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
