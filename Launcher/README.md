# Launcher

`Launcher.exe` — the companion app you run to inject the in-game module. It is a small
**console injector/bootstrapper**: it loads `Internal.dll`, forces Windows to map it into the
running game, hands off the hook handle, and exits. All in-game behavior lives in
[`Internal`](../Internal); the launcher's only job is process/window discovery + installing the
Windows hook.

## How it works

The launcher uses the classic `SetWindowsHookEx` injection technique:

1. `LoadLibraryA("Internal.dll")` and resolve the exported `SplitgateCallBack` via
   `GetProcAddress`. The export is **not** `extern "C"`, so it is resolved by its C++-**mangled**
   name.
2. Find the game window (`PortalWars`) and get its UI thread + process id.
3. Install a **`WH_GETMESSAGE`** hook (`SetWindowsHookExW`) on that thread pointing at
   `SplitgateCallBack` inside `Internal.dll`. This forces Windows to map the DLL into the **game
   process**.
4. `PostThreadMessageW` a trigger message whose `lParam` carries the returned `HHOOK`, then exit.

Inside the game process the DLL takes over from there (`Hook::Init()`), and the launcher
deliberately does **not** unhook — that could unload the DLL. The DLL owns teardown.

Both sides agree on the trigger message via `RegisterWindowMessageW(L"SplitgateInit")`. See
[docs/hooking.md](../docs/hooking.md) for the full breakdown.

## Layout

| Path | Contents |
|------|----------|
| `Launcher.cpp` | Entry point / injector logic (window + thread discovery, hook install, handle handoff). |
| `app.manifest` | Win32 application manifest. |
| `data/` | Win32 resources — `Logo.rc` (compiled), `Logo.h`, `logo.ico` (app icon). |
| `scripts/` | Bundled mitmproxy addons (`default_proxy.py`, `watchdog.py`), copied next to `Launcher.exe` at build time and used by `utils/Mitmproxy.h`. Fed inputs via environment variables (`SPLITGATE_REDIRECTS`, `SPLITGATE_GAME_PID`) so they stay static. |
| `utils/Logger.h` | Thin `Logger` over [`shared/Logger.h`](../shared/Logger.h), bound to the console and `launcher.log`. |
| `utils/ExceptionHandler.h` | Launcher-side wiring of the shared crash handler. |
| `utils/Mitmproxy.h`, `utils/ProxyConfig.h` | Proxy launching (`ProxyMode::Mitmproxy`) and its configuration. |
| `utils/handles/` | RAII wrappers — `UniqueHandle.h`, `UniqueHook.h`, `UniqueLibrary.h`. |

## Build

- **Console** application (`SubSystem=Console`), root namespace `Launcher`, x64 **Release** only
  (`v145`, C++ latest, Unicode).
- Depends on `nlohmann-json` via **vcpkg** (manifest mode — see `vcpkg.json`); no linked libs
  beyond the Windows SDK.
- Ships alongside `Internal.dll` and the copied `scripts/` folder.

See the root [README](../README.md#compile) for full build instructions.
