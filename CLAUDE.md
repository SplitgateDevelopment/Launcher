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

Other top-level items: `Tools/` (build scripts, incl. `build.bat` used by CI, and
`format.ps1` for clang-format), `.github/workflows/msbuild.yml` (CI),
`.clang-format` / `.clang-tidy` (style/lint config), `docs/` (see below), `README.md`.

## Documentation

Longer-form docs live in [`docs/`](docs/) and are linked from the README:

- [docs/settings.md](docs/settings.md) — configuration structs, persistence, file location.
- [docs/features.md](docs/features.md) — the feature framework and how to add a feature.
- [docs/scripting.md](docs/scripting.md) — embedding Python and writing user scripts.
- [docs/testing.md](docs/testing.md) — the gtest project and how to run it.
- [docs/style.md](docs/style.md) — the clang-format / clang-tidy setup.

## Usage (from README)

Put `Launcher.exe` and `Internal.dll` in the same folder, start the game, open
`Launcher.exe`, then press **Ins** to show/hide the GUI. Prebuilt binaries are published
via nightly.link; otherwise build from source.

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

- `Launcher.cpp` — entry point / launcher logic.
- `utils/Logger.h` — a `Logger` class: `error`/`success`/`info` print `[LEVEL] msg` to stdout
  (`std::format`); `errorBox(fn)` pops a Win32 `MessageBox` with the `GetLastError()` text;
  `stop(code)` does the "press any key to exit" console wait. Uses WinAPI directly.
- `data/` — Win32 resources: `Logo.rc` (compiled), `Logo.h`, `logo.ico` (the app icon);
  `.aps` files are the resource editor's cache.

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
- `utils/` — helpers (`Globals.h`, `Logger.h`, `ExceptionHandler.h`, `Util.h/.cpp`).
- `dllmain.cpp` — DLL entry point.

## Conventions

See [.claude/rules/code-style.md](.claude/rules/code-style.md) — PascalCase file/namespace/
class names, camelCase variables, lowercase directories, reusable namespaces.
