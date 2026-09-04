# Planned features

Design notes for five features agreed for the `feat/aim-profiles-hooks` branch. Nothing here is
implemented yet — this is the plan. Each feature lands as its own focused commit (per the
[Version control](../CLAUDE.md#version-control) convention). Suggested order is small/independent
first: **1 → 2 → 3 → 5 → 4**.

Legend for "forbidden files": `Internal/dllmain.cpp` and `Launcher/Launcher.cpp` can't be edited
here; any change they need is captured in `TODO.md` instead.

---

## 1. Real minidumps in the crash handler

**Goal.** The shared crash handler currently writes a text stack trace
(`Crashes/<timestamp>/StackTrace.log`). Also write a real `.dmp` next to it so a crash can be
opened in a debugger (Visual Studio / WinDbg) with full call stacks, locals, and module list.

**Approach.**
- In [`shared/ExceptionHandler.h`](../shared/ExceptionHandler.h), after `WriteStackTrace`, call
  `MiniDumpWriteDump` writing `Crashes/<timestamp>/Crash.dmp`, passing the live
  `EXCEPTION_POINTERS` via a `MINIDUMP_EXCEPTION_INFORMATION` so the dump is anchored at the fault.
- Add a dump-type to `Config` (e.g. `MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory |
  MiniDumpWithDataSegs` for a "small" dump; a heavier flag set for a "full" dump), defaulting to
  the small one. Keep it a `Config` field so launcher and DLL can differ if wanted.
- `DbgHelp.lib` is already linked (`#pragma comment` in the same header); `MiniDumpWriteDump` and
  `MINIDUMP_TYPE` come from `<DbgHelp.h>`, already included.
- The `WriteCrashLog` signature already carries the exception code + `CONTEXT*`; thread the
  `EXCEPTION_POINTERS*` through `Filter` so the dump gets the real record (today only `CONTEXT` is
  passed). Pure enough that the existing `ExceptionHandlerTests` can assert the `.dmp` is created.

**Touches.** `shared/ExceptionHandler.h` (+ a test in `Tests/ExceptionHandlerTests.cpp`).
**Forbidden files.** None — both sides already call `Install`/`WriteCrashLog` through the shared
API.
**Size.** Small.

---

## 2. Streamproof overlay (toggle)

**Goal.** Optionally hide the in-game overlay (menu + ESP) from screen capture (OBS, Discord,
Xbox Game Bar) so only the local player sees it.

**Approach.**
- `SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE)` on the game window
  (`Window::WindowHandle`, resolved in [`menu/gui/Gui.h`](../Internal/menu/gui/Gui.h) as
  `FindWindow(L"UnrealWindow", L"PortalWars  ")`). `WDA_NONE` restores normal capture.
- New `MenuSettings::Streamproof` bool (persisted). Apply it once at `GUI::Init()` and whenever the
  toggle flips.
- UI: a "Streamproof" toggle in the **Misc** section (`menu/sections/Misc.h`). Dispatch
  `SettingsChanged` like the other toggles.
- Caveat to document: `WDA_EXCLUDEFROMCAPTURE` needs Windows 10 2004+ (older returns an error and
  the window just stays visible in captures); the affinity applies to the whole game window, not
  just the overlay layer.

**Touches.** `settings/Settings.h`, `menu/sections/Misc.h`, and the apply-site in `menu/gui/Gui.h`.
**Forbidden files.** None.
**Size.** Small.

---

## 3. Guard-page hook (`Hook::GuardHook`) — prototype

**Goal.** A code-patch-free hooking primitive as an alternative to MinHook, extrapolated from the
Platanium project's technique. MinHook patches the target's prologue (detectable by a
code-integrity check); a guard-page hook modifies **no** bytes.

**Approach.** New header `hook/GuardHook.h`, `namespace Hook` (our own implementation):
- `Install(target, detour)` marks `target`'s page `PAGE_GUARD` and registers a vectored exception
  handler with `AddVectoredExceptionHandler`.
- On `EXCEPTION_GUARD_PAGE` at `target`, the handler rewrites `ContextRecord->Rip` to `detour` and
  sets the trap flag (`EFlags |= 0x100`) so the next instruction single-steps.
- On the following `EXCEPTION_SINGLE_STEP`, re-arm `PAGE_GUARD` (accessing a guard page clears it),
  so subsequent calls keep trapping.
- `Remove()` restores page protection and removes the VEH handler; track installed hooks in a small
  table keyed by target address.
- Trade-offs to document in [`docs/hooking.md`](hooking.md): an exception per call (slow — fine for
  rarely-called functions, not per-frame `PostRender`), hooks by address so it needs the resolved
  function pointer, and it's more fragile than a trampoline. Ship it opt-in with a demo hook behind
  a Debug-tab switch rather than replacing the MinHook path.
- Interplay with the existing crash handler: the process's `SetUnhandledExceptionFilter` is
  last-chance; `AddVectoredExceptionHandler(1, ...)` runs first, so `GuardHook` intercepts its own
  guard/step faults before the crash handler sees them — verify they coexist.

**Touches.** `hook/GuardHook.h` (new), `docs/hooking.md`, a Debug-tab demo toggle.
**Forbidden files.** None (the primitive is self-contained; wiring a real hook through it is done in
readable `hook/` code).
**Size.** Medium.

---

## 4. Aimbot + triggerbot, and a dedicated Aim tab

**Goal.** Two new features that reuse the ESP's world→screen projection and bone resolution, plus a
new **Aim** menu tab that gathers the aim-related controls.

**Approach.**
- Two new `Feature`s under `features/` (`Aimbot.h`, `Triggerbot.h`), following the existing
  `Feature` contract (Init/Check/Run/Destroy, the `Events::Type` they run on — `Render` for aim
  math each frame). Reuse the ESP's actor/bone/`WorldToScreen` helpers rather than re-deriving them.
  - **Aimbot:** while the aim key is held, pick the target whose screen position is nearest the
    crosshair within a configurable FOV radius, resolve the chosen bone, and move the aim toward it
    with a smoothing factor. Respect team checks / visibility if the ESP already computes them.
  - **Triggerbot:** when the crosshair is already on an enemy (reuse the same target test), fire
    after a configurable delay while the trigger key is held.
- New `AimSettings` struct in `settings/Settings.h` (persisted): `Aimbot`/`Triggerbot` enables, aim
  key + trigger key, FOV radius, smoothing, target bone, triggerbot delay, team/visible filters.
- New `menu/sections/Aim.h` rendering those controls; add an **Aim** tab to the menu tab bar in
  `menu/Menu.h`. Move any aim-adjacent controls currently elsewhere (e.g. anything under Exploits
  that's really aim) into this tab.
- Register both features in `features/Features.h` (and wire the `SettingsChanged` refresh like the
  others).

**Touches.** `features/Aimbot.h`, `features/Triggerbot.h`, `features/Features.h`,
`settings/Settings.h`, `menu/sections/Aim.h`, `menu/Menu.h`; docs in
[`docs/features.md`](features.md).
**Forbidden files.** None expected — features and menu sections are readable. If the tab-bar
registration turns out to sit in a forbidden file, that one line goes to `TODO.md`.
**Size.** Large.

---

## 5. Config profiles / share codes (dedicated page)

**Goal.** Multiple named settings profiles, plus import/export as portable "share codes" so a
config can be pasted between users.

**Approach.**
- Profiles are separate settings files under the app folder: `profiles/<name>.settings`, each a
  full `SETTINGS` document loaded/saved through `Shared::SettingsFile<SETTINGS>` (the generic
  persistence we already have) — no new serialization.
- A small `settings/Profiles.h` helper: `List()` (enumerate `profiles/*.settings`), `Save(name)`,
  `Load(name)` (into the live `Settings`, then dispatch `SettingsChanged`), `Delete(name)`.
- **Share codes:** serialize the live `SETTINGS` to JSON, then base64 that string. Export copies the
  code to the clipboard; import decodes + parses (tolerant, like normal load) and applies it. Add a
  tiny base64 helper in `utils/` (or reuse one if present); JSON round-trip already exists via
  nlohmann.
- New `menu/sections/Profiles.h` page + a **Profiles** tab in `menu/Menu.h`: list existing profiles
  (load / delete), a "save as" field, and copy/paste boxes for the share code.
- Guard rails to document: importing overwrites the current config (offer a confirm), and runtime-
  only fields (the ones already excluded from the persistence macros) are intentionally not carried
  by a share code.

**Touches.** `settings/Profiles.h` (new), `menu/sections/Profiles.h` (new), `menu/Menu.h`, a base64
helper in `utils/`; docs in [`docs/settings.md`](settings.md).
**Forbidden files.** None expected (same tab-bar caveat as #4).
**Size.** Medium.

---

## 6. Switchable renderer: ImGui/DX11 vs UE Canvas (runtime toggle)

**Would ImGui be faster? Yes, substantially.** Today the visual features draw through the UE
canvas — `UCanvas::K2_DrawLine` / `K2_DrawText` — and **every** one of those is a `ProcessEvent`
(UFunction) call. A skeleton alone is 16 lines = 16 `ProcessEvent`s per enemy per frame; boxes,
names, health and snaplines add more. ImGui's `ImDrawList::AddLine` / `AddText` are essentially
free (they append to a vertex buffer, no `ProcessEvent`), so an ImGui-drawn ESP would cut the
per-primitive cost dramatically — the natural companion to the ActorCache and the cached
projection.

**Goal.** A `Settings.VISUALS.Renderer` enum (`Canvas` default, `ImGui`), switchable live from the
Visuals tab, that chooses how the ESP/radar/debug overlays are drawn.

**Approach — yes, an OOP abstract renderer is exactly right.** A `render/` module with an abstract
base `class Renderer { virtual void Line(a, b, thickness, color) = 0; virtual void Text(pos, text,
scale, color) = 0; };` and **two concrete implementations** — `CanvasRenderer` (`Globals::Canvas->
K2_Draw*`) and `ImGuiRenderer` (`ImDrawList::Add*`). A single `Render::active` pointer (swapped from
the setting, live) is what the features call, so ESP/radar/debug are backend-agnostic and build
their boxes/skeletons out of `Render::active->Line(...)`. Classic strategy pattern.
- **The timing gotcha to design around:** canvas draws happen in **PostRender** (the UE hook);
  ImGui draws must happen during the **ImGui frame in the Present hook** (between `NewFrame()` and
  `Render()`), and its draw lists reset each frame. Cleanest fit for this OOP split: `ImGuiRenderer`
  **records** each `Line`/`Text` into a per-frame command buffer during PostRender, and
  `GUI::Overlay` replays that buffer into `GetBackgroundDrawList()` after `NewFrame()`, then clears
  it — so feature logic stays in one place (PostRender) and only the *backend* differs.
  `CanvasRenderer` draws immediately. Coordinates come from the cached `ProjectWorldLocationToScreen`
  in both modes.
- **This also fixes streamproof properly.** `WDA_EXCLUDEFROMCAPTURE` on the *game* window hides the
  whole game from capture (the current bug). To hide only the overlay, `ImGuiRenderer` renders into
  a separate, transparent, click-through, top-most overlay window that has the exclude-from-capture
  affinity, while the game window stays captured. So streamproof implies the ImGui renderer — the
  two ship together.
- **Color/coords** already exist as `FLinearColor` + `FVector2D`; the ImGui backend converts to
  `ImU32` / `ImVec2`.
- A **native WorldToScreen** (matrix math from the camera POV, no `ProcessEvent`) is a further,
  independent optimization that would speed up *both* backends — worth noting but separable.

**Touches.** `settings/Settings.h` (Renderer enum), a `Render` abstraction, `features/Esp.h`,
`features/Radar.h`, `features/DebugNames.h` (call `Render::*`), `menu/gui/Gui.h` (drive features in
ImGui mode), `menu/sections/Visuals.h` (the toggle).
**Forbidden files.** None expected.
**Size.** Large (touches every draw call + the render-loop timing).

---

## Cross-cutting notes

- **Settings growth.** Features 2/4/5 add persisted fields/sections; keep the
  `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT` macros in sync and add round-trip cases to
  `Tests/SettingsTests.cpp` for the new fields (missing-key tolerance already covers older files).
- **Menu tabs.** Features 4 and 5 both add a top-level tab. Confirm where the tab bar is declared
  (`menu/Menu.h` vs a forbidden file) before starting either, so any unavoidable forbidden edit is
  batched into one `TODO.md` entry.
- **Testing.** 1 and 5 have unit-testable cores (dump-created assertion; base64 + JSON round-trip);
  2/3/4 need in-game verification.
