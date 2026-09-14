# Internal

`Internal.dll` — the in-game module. Once [`Launcher.exe`](../Launcher) maps it into the
Splitgate process, the DLL drives everything: it hooks the engine, renders an ImGui/UE-Canvas
overlay through **DirectX 11**, runs the gameplay features, embeds **CPython** for user scripting,
and reports Discord Rich Presence. Root namespace `Splitgate`; exports are gated behind
`SPLITGATE_EXPORTS`.

## Startup

`dllmain.cpp` exports the `WH_GETMESSAGE` hook procedure the launcher installs,
`SplitgateCallBack`. When the trigger message arrives it captures the `HHOOK` into
`Hook::injectionHook`, then (once, guarded by `Hook::initialized`) runs **`Hook::Init()`**, which:

- opens the console and initializes the engine (`Engine::Init` — signature scan) and `Globals`;
- walks `World → OwningGameInstance → LocalPlayers[0] → ViewportClient → VFTable` to capture the
  `PostRender` and `ProcessEvent` vtables;
- seeds settings from the game, starts Python (`Scripts::Init()`);
- installs the hooks (MinHook for `ProcessEvent`, a vtable swap for `PostRender`);
- initializes the GUI (`GUI::Init()`) and registers features (`Features::Init()`).

From then on `PostRender` renders the menu + features each frame and `ProcessEvent` feeds the
event bus. Teardown lives in `Hook::UnHook()`. See [docs/hooking.md](../docs/hooking.md).

## Layout

| Path | Contents |
|------|----------|
| `hook/` | The hooking engine (`Hook.h`) and the two hooked game functions in `hook/functions/` (`ProcessEvent.h`, `PostRender.h`). |
| `features/` | Feature framework (`Feature.h`, `FeatureRunner.h`, `Features.h`) and the concrete features (GodMode, InfiniteJetpack, NoRecoil, SpinBot, ESP, Radar, FreeCam, ThirdPerson, …). |
| `menu/` | The GUI (`Menu.h`), with `gui/`, `sections/`, and two rendering backends — `backend/` + `canvas/` (UE-Canvas / ZeroGUI) behind the `ui/` facade. |
| `render/` | Backend-neutral render layer — `Render::Vec2`/`Color` and trait `adapters/` (ImGui, UE Canvas, Settings). |
| `ue/` | The Unreal Engine SDK (`sdk/`, `Engine.h`, `custom.*`) and `namespace Engine` (scanned internals + resolved game objects). |
| `scripting/` | Python via pybind11 (`Scripts.h`) and the C++ event bus (`Events.h`), exposing `modules/` to user scripts. |
| `network/` | Backend redirect — redirect map, HTTP logger, WinHTTP + libcurl hooks. |
| `cache/` | Lookup caches (`ActorCache`, `ClassCache`, `FontCache`, `NameCache`). |
| `native/` | Low-level helpers (`WorldToScreen`, `Visibility`, `ActorLocation`). |
| `memory/` | Self-written memory/signature utilities (`Memory.h`). |
| `discord/` | Discord Rich Presence (`rpc.h`, `handlers.h`). |
| `settings/` | Configuration (`Settings.h/.cpp`). |
| `utils/` | Helpers and thin facades over [`shared/`](../shared) (`Logger.h`, `ExceptionHandler.h`). |
| `dllmain.cpp` | DLL entry point and the exported `SplitgateCallBack` hook procedure. |

## Build

- **DynamicLibrary**, x64 **Release** only (`v145`, C++ latest). Links `d3d11.lib`.
- vcpkg (manifest mode, `vcpkg.json`): `imgui` (dx11 + win32 bindings), `minhook`,
  `nlohmann-json`. Submodules under `external/`: `pybind11`, `discord-rpc`.
- Embeds **CPython** (pybind11 + `<Python.h>`), so a Python 3.x install is required to compile.
  The Python root is resolved by [`python.props`](python.props); the chosen version must match the
  `pythonXX.dll` present at runtime.

See the root [README](../README.md#compile) for full build instructions, and
[docs/](../docs) for the module guides.
