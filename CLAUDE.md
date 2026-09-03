# Splitgate

Custom launcher and in-game DLL for the Unreal Engine game **Splitgate**, written in C++.
The DLL is injected/loaded alongside the game and renders an in-game menu that drives a set
of gameplay features; the launcher is the companion executable used to start it.

## Repository layout

The Visual Studio solution (`.sln` at the repo root) contains two projects:

- **Launcher** — produces `Launcher.exe`, the companion app the user runs. *(details below)*
- **Internal** — produces `Internal.dll`, the in-game module. *(details below)*

Other top-level items: `Tools/` (build scripts, incl. `build.bat` used by CI),
`.github/workflows/msbuild.yml` (CI), `README.md`.

## Usage (from README)

Put `Launcher.exe` and `Internal.dll` in the same folder, start the game, open
`Launcher.exe`, then press **Ins** to show/hide the GUI. Prebuilt binaries are published
via nightly.link; otherwise build from source.

## Build

- Toolchain: **Visual Studio 2022** (`v143`), C++ latest standard, x64 **Release** only.
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
(`v143`, C++ latest, Unicode). No vcpkg/external dependencies and no extra linked libs — it's
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

- `hook/` — hooking engine (`Hook.h`) and the feature framework (`Feature.h`, `Features.h`).
  - `hook/functions/` — hooked game functions (`ProcessEvent.h`, `PostRender.h`).
  - `hook/features/` — individual features: DrawActors, GodMode, InfiniteJetpack,
    LoadIntoMap, NoRecoil, PlayerModifications, SpinBot, UserScripts, WeaponModifications.
- `menu/` — the GUI (`Menu.h`), with `gui/` (Config, Custom, Gui, Styles, Window) and
  `sections/` (Debug, Exploits, Misc, Settings, Watermark).
- `scripting/` — Python scripting via pybind11 (`Scripts.h`), exposing `modules/`
  (Logger, Settings) to user scripts.
- `ue/` — Unreal Engine SDK (`Engine.h/.cpp`, `UObjects.h`).
- `discord/` — Discord Rich Presence integration (`rpc.h`, `handlers.h`).
- `settings/` — configuration (`Settings.h/.cpp`).
- `utils/` — helpers (`Globals.h`, `Logger.h`, `ExceptionHandler.h`, `Util.h/.cpp`).
- `dllmain.cpp` — DLL entry point.

## Conventions

See [.claude/rules/code-style.md](.claude/rules/code-style.md) — PascalCase file/namespace/
class names, camelCase variables, lowercase directories, reusable namespaces.
