# Splitgate

Custom launcher and in-game DLL for the Unreal Engine game **Splitgate**, written in C++.
The DLL is injected/loaded alongside the game and renders an in-game menu that drives a set
of gameplay features; the launcher is the companion executable used to start it.

> [!IMPORTANT]
> **Do not read these files** — `Internal/dllmain.cpp`, `Launcher/Launcher.cpp`. If you
> need something from one of them, ask and it will be provided to you.
>
> **`Internal/hook/Hook.h` is partially readable**: you may read lines **1–17** (includes,
> `namespace Hook` opening, `g_hook`, and the `SetHook` signature) and lines **34 to the end**
> (`Init` / `UnHook` / `isKeyPressed`). **Never read lines 18–33** — the body of
> `BYTE *SetHook(void **VTable, int index, void *TargetFunction)`.

## Repository layout

The Visual Studio solution (`.sln` at the repo root) contains three projects:

- **Launcher** — produces `Launcher.exe`, the companion app the user runs. *(details below)*
- **Internal** — produces `Internal.dll`, the in-game module. *(details below)*
- **Tests** — a GoogleTest console `.exe` (vcpkg `gtest`) covering the self-contained modules
  (settings, feature framework, event bus). See [docs/testing.md](docs/testing.md).

Other top-level items: `shared/` (headers used by **both** projects — `Ipc.h` for the
launcher/DLL init handshake, `Logger.h` for the shared console logger, `ExceptionHandler.h`
for the reusable crash handler, `Settings.h` for the generic `SettingsFile<T>` persistence,
`LauncherSettings.h` for the launcher-only settings the DLL's Network tab edits and the
launcher reads), `Tools/` (build
scripts, incl. `build.bat` used by CI, and `format.ps1` for clang-format),
`.github/workflows/msbuild.yml` (CI), `.clang-format` / `.clang-tidy` (style/lint config),
`docs/` (see below), `README.md`.

## Documentation

Longer-form docs live in [`docs/`](docs/) and are linked from the README:

- [docs/settings.md](docs/settings.md) — configuration structs, persistence, file location.
- [docs/features.md](docs/features.md) — the feature framework and how to add a feature.
- [docs/hooking.md](docs/hooking.md) — injection (the `WH_GETMESSAGE` technique, the mangled
  `SplitgateCallBack` export) and how the game's functions are hooked (`PostRender`,
  `ProcessEvent`).
- [docs/debugging.md](docs/debugging.md) — reading logs (`launcher.log` / `internal.log`), the
  in-game console, crash stack traces, and the Debug-tab switches.
- [docs/game-dump.md](docs/game-dump.md) — the Dumpspace SDK dump for Splitgate (hash
  `d2a5bd8c`): the JSON files/schema, the GNames/GObjects/GWorld offsets, and how the reference
  `DSAPI` wrapper consumes them.
- [docs/backend-redirect.md](docs/backend-redirect.md) — redirecting the game to a self-hosted
  backend (the `network/` module: redirect map, HTTP logger, WinHTTP + libcurl hooks), target
  configurable via settings JSON; with scope/legal caveats.
- [docs/early-injection.md](docs/early-injection.md) — design note on covering the backend calls
  the game makes before the DLL is injected (suspended-launch + early injection; not built).
- [docs/scripting.md](docs/scripting.md) — embedding Python and writing user scripts.
- [docs/testing.md](docs/testing.md) — the gtest project and how to run it.
- [docs/style.md](docs/style.md) — the clang-format / clang-tidy setup.
- [docs/planned-features.md](docs/planned-features.md) / [docs/roadmap.md](docs/roadmap.md) — design
  notes for near-term features and larger future work (not yet implemented).

## Usage (from README)

Put `Launcher.exe` and `Internal.dll` in the same folder, start the game, open
`Launcher.exe`, then press **Ins** to show/hide the GUI. Prebuilt binaries are published
via nightly.link; otherwise build from source.

## Injection & startup flow

The launcher gets `Internal.dll` running inside the game via the classic `SetWindowsHookEx`
injection technique, then the DLL takes over:

1. **Launcher** loads `Internal.dll`, resolves the exported `SplitgateCallBack` (via its
   *mangled* name, since it is not `extern "C"`), finds the `PortalWars` window and its UI
   thread, and installs a **`WH_GETMESSAGE`** hook on that thread whose proc is
   `SplitgateCallBack`. This forces Windows to map `Internal.dll` into the **game process**.
   It then `PostThreadMessageW`s a trigger message whose `lParam` carries the returned
   `HHOOK`, and exits.
2. Inside the game process, `SplitgateCallBack` (`dllmain.cpp`) runs when that message is
   pulled from the queue. It captures the `HHOOK` from `msg->lParam` into `Hook::g_hook` (so
   `UnHook()` can later remove the hook), guards against re-entry with `Hook::g_initialized`,
   and calls **`Hook::Init()`** (see `hook/Hook.h`), which installs the `ProcessEvent` /
   `PostRender` hooks, the GUI, Python, and the features. `CallNextHookEx`'s first argument is
   ignored by Windows, so a not-yet-set `g_hook` there is harmless — the handle only matters
   for the eventual `UnhookWindowsHookEx`.
3. From then on the DLL drives everything: `PostRender` renders the menu + features each
   frame, `ProcessEvent` feeds the event bus, and Discord RPC runs in the background. The
   launcher deliberately does **not** unhook (that could unload the DLL); the DLL owns
   teardown in `Hook::UnHook()`.

So the launcher is only an injector (process/window discovery + hook install + handle
handoff); all in-game behavior lives in the DLL. Both sides agree on the trigger message via
`RegisterWindowMessageW(L"SplitgateInit")` (a session-unique id, no shared constant needed) —
see [docs/hooking.md](docs/hooking.md).

## Build

- Toolchain: **Visual Studio 2022** (`v145`), C++ latest standard, x64 **Release** only.
- Dependencies via **vcpkg** (manifest mode). `Internal` also uses submodules under
  `Internal/external/` (pybind11, discord-rpc).
- `Internal` embeds **CPython** (pybind11 + `<Python.h>`) for user scripting, so a Python
  3.x install is required to compile it. The Python root is resolved by
  [`Internal/python.props`](Internal/python.props) (imported from the `.vcxproj`), which
  tries in order: `/p:PythonRoot=`, the `SPLITGATE_PYTHON_DIR` env var, well-known install
  dirs newest-first, then the installer's registry `InstallPath`; it injects `include`/`libs`
  before ClCompile/Link and errors if none is found. `pythonXX.lib` is auto-linked via
  `pyconfig.h`, so no explicit lib entry is needed — but the chosen version must match the
  `pythonXX.dll` at runtime.
- Links `d3d11.lib` — the in-game overlay renders through **DirectX 11**.
- CI (`.github/workflows/msbuild.yml`) builds on `windows-latest`: `setup-python` (3.14, passed
  to the build via `SPLITGATE_PYTHON_DIR`), vcpkg bootstrap + integrate, `nuget restore`, then
  `Tools/build.bat`, and uploads `x64/Release`. Triggers on manual dispatch and on push/PR to
  `master` touching `Internal/**` or `Launcher/**`.

## Launcher (`Launcher.exe`) — structure

**Console** application (`SubSystem=Console`), root namespace `Launcher`, x64 Release only
(`v145`, C++ latest, Unicode). No vcpkg/external dependencies and no extra linked libs — it's
self-contained relative to `Internal`. Preprocessor: `_CRT_SECURE_NO_WARNINGS;NDEBUG;_CONSOLE`.

- `Launcher.cpp` — entry point / launcher logic. It is an **injector/bootstrapper**:
  `LoadLibraryA("Internal.dll")`, resolves the exported `SplitgateCallBack` via
  `GetProcAddress` (using the C++-mangled symbol name, as the export is not `extern "C"`),
  finds the game window (`PortalWars`), gets its UI thread + process id, installs a
  `WH_GETMESSAGE` hook (`SetWindowsHookExW`) pointing at that callback inside `Internal.dll`,
  posts a thread message (with the `HHOOK` in its `lParam`) to trigger it, then exits. Its
  whole job is process/window discovery + installing the Windows hook + handing off the
  handle. It does not unhook (the DLL owns that).
- `utils/Logger.h` — a thin `Logger` subclass of `Shared::Logger` (`shared/Logger.h`) bound
  to the launcher's existing console and `launcher.log`. All logging behavior (leveled/colored
  output, file mirroring with a local-time `[HH:MM:SS]` prefix, `errorBox`, `stop`) lives in
  the shared class.
- `data/` — Win32 resources: `Logo.rc` (compiled), `Logo.h`, `logo.ico` (the app icon);
  `.aps` files are the resource editor's cache.
- `scripts/` — bundled mitmproxy addon `.py` files (`default_proxy.py`, `watchdog.py`) used by
  `utils/Mitmproxy.h` in `ProxyMode::Mitmproxy`. Copied next to `Launcher.exe` (into a `scripts/`
  folder) at build time; fed their inputs via environment variables (`SPLITGATE_REDIRECTS`,
  `SPLITGATE_GAME_PID`) so they stay static, editable Python.

## Internal (`Internal.dll`) — structure

DynamicLibrary, root namespace `Splitgate`, exports gated behind `SPLITGATE_EXPORTS`.
Source folders (from the project file; contents documented as they are read):

- `hook/` — the hooking engine. `Hook.h` defines `namespace Hook` and drives the module's
  whole lifecycle:
  - `SetHook(VTable, index, TargetFunction)` — swaps a single vtable entry and returns the
    original pointer (used for the manual `PostRender` hook).
  - `Init()` — the one-time bootstrap run on injection: opens the console, initializes the
    engine/`Globals`, walks `World → OwningGameInstance → LocalPlayers[0]
    (UPortalWarsLocalPlayer) → ViewportClient → VFTable` to capture the `PostRender` and
    `ProcessEvent` vtables, seeds settings from the game (`FOV` from the save game, then
    `SettingsHelper::Load()`), spawns a `UConsole`, starts Python (`Scripts::Init()`),
    installs the hooks (MinHook for `ProcessEvent`, vtable swap for `PostRender`),
    initializes the ImGui GUI (`GUI::Init()`), and registers features (`Features::Init()`).
  - `UnHook()` — teardown: disables/uninitializes MinHook, restores the `PostRender` vtable
    entry, destroys the console/GUI, disables the exception handler, removes the Win32 hook.
  - `isKeyPressed(key)` — edge-detected key poll (just-pressed **and** held) for the `Ins`
    menu toggle and hotkeys.
  - `hook/functions/` — the two hooked game functions: `ProcessEvent.h` (funnels UE events,
    driving the event bus) and `PostRender.h` (per-frame, drives `Features::Execute()` and
    the menu).
- `features/` — the feature framework and the individual features (moved here out of
  `hook/`).
  - `Feature.h` — the base `Feature` type (Init/Check/Run/Destroy, Enabled/OneTime state,
    and the `Events::Type` the feature runs on).
  - `FeatureRunner.h` — the `Features` registry and per-feature tick (`RunFeature`) plus the
    render-loop `Execute()`.
  - `Features.h` — includes the concrete features, and `Init()` registers them and wires
    them to the event bus (including the `SettingsChanged` refresh handler and the
    `LoadIntoMap` handler).
  - Concrete features: `GodMode`, `InfiniteJetpack`, `NoRecoil`, `SpinBot`,
    `PlayerModifications`, `WeaponModifications`, `UserScripts`, `ThirdPerson`, `FreeCam`,
    `Radar`, `Esp`. (The former `DrawActors` and `LoadIntoMap` features were removed —
    ESP replaces the former; LoadIntoMap is now a menu-dispatched event.)
- `menu/` — the GUI (`Menu.h`), with `gui/` (Config, Custom, Gui, Styles, Window) and
  `sections/` (Debug, Exploits, Misc, Settings, Visuals, Watermark — `Visuals` configures the
  ESP elements: 2D/3D boxes, bones, name, snaplines, health, distance, and their colors).
- `scripting/` — Python scripting via pybind11 (`Scripts.h`) and the C++ event bus
  (`Events.h`: `Events::Type` enum, `Events::Payload`, Register/Dispatch). Exposes `modules/`
  (Logger, Settings, Events) to user scripts.
- `ue/` — Unreal Engine SDK (`Engine.h/.cpp`, `UObjects.h`).
- `discord/` — Discord Rich Presence integration (`rpc.h`, `handlers.h`).
- `settings/` — configuration (`Settings.h/.cpp`).
- `utils/` — helpers (`Globals.h`, `Util.h/.cpp`, plus two facades over `shared/`: `Logger.h`
  is a `namespace Logger` facade (`Log`/`CreateConsole`/`DestroyConsole`/`SetConsoleVisibility`)
  over one `Shared::Logger` that spawns the in-game console and logs to `internal.log`; and
  `ExceptionHandler.h` wires `Shared::ExceptionHandler` with the game's crash folder, the
  logger, and `SettingsHelper::Delete` as the recovery hook, keeping the `Init()`/`Disable()`
  surface unchanged).
- `dllmain.cpp` — DLL entry point. Exports the `WH_GETMESSAGE` hook procedure the launcher
  installs — `LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam)`
  (`lparam` is the `MSG*`, valid only when `code >= 0`). It is **not** `extern "C"`, so the
  launcher resolves its **mangled** export name `?SplitgateCallBack@@YA_JH_K_J@Z`
  (`__int64 __cdecl SplitgateCallBack(int, unsigned __int64, __int64)` on x64) — see
  [docs/hooking.md](docs/hooking.md) for the full breakdown; changing the signature breaks
  that lookup. On the trigger message it captures the `HHOOK` from `msg->lParam`
  into `Hook::g_hook`, then (once, guarded by `Hook::g_initialized`) initializes the
  `ExceptionHandler`, runs `Hook::Init()` (the bootstrap described above), logs the
  injection + module base address + menu hotkey, initializes Discord RPC, and chains to
  `CallNextHookEx`. `Hook::g_hook` / `Hook::g_initialized` are declared in `hook/Hook.h`.

## Conventions

See [.claude/rules/code-style.md](.claude/rules/code-style.md) — PascalCase file/namespace/
class names, camelCase variables, lowercase directories, reusable namespaces, and
always-early-return control flow.

## Version control

- Prefer **many small, focused commits** over one large one — split unrelated changes (a fix,
  a refactor, docs) into separate commits so each is reviewable and revertable on its own.
- Write commit messages in the **[Conventional Commits](https://www.conventionalcommits.org/)**
  style: `type(scope): summary` (e.g. `fix(hook): capture the injection hook handle`,
  `docs: add docs/hooking.md`). Common types: `feat`, `fix`, `docs`, `refactor`, `chore`,
  `test`.
