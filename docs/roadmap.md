# Roadmap

Design notes for larger future work, beyond the near-term [planned-features.md](planned-features.md)
(minidumps, streamproof, guard-hook, aimbot, profiles, renderer). Nothing here is implemented —
this is the plan. Grouped by theme; each entry has an approach, the files it touches, a rough size,
and dependencies. "Forbidden files" = `Internal/dllmain.cpp` / `Launcher/Launcher.cpp`, whose edits
go to `TODO.md`.

---

## Performance

### Native WorldToScreen

**Goal.** Replace `ProjectWorldLocationToScreen` (a `ProcessEvent` per point) with matrix math, so
projection costs nothing. The ESP's skeleton is ~32 projections per enemy per frame — this is the
single biggest projection win, independent of the renderer.

**Approach.**
- Read the camera POV without a UFunction: `Globals::PlayerController->PlayerCameraManager` exposes a
  cached POV (`CameraCachePrivate`/`ViewTarget` POV) with `Location`, `Rotation`, `FOV`. Confirm the
  offset in the SDK dump (or read it via `GetCameraLocation`/`GetCameraRotation` once and cache).
- Build the standard UE world→screen: rotation matrix from the camera `Rotation` (forward/right/up),
  transform `world - camera`, perspective-divide by the forward component using `tan(FOV/2)` and the
  viewport size (`Canvas->ClipX/ClipY`, already available). Return `false` when behind the camera.
- `bool WorldToScreen(const FVector& world, FVector2D& out)` in `ue/` (or a `math/` helper). Swap the
  bone projection (`USkeletalMeshComponent::GetBone` = `GetBoneMatrix` native + project) and the ESP
  3D-box/name/distance projections to use it. `GetBoneMatrix` is already native, so bones become
  fully `ProcessEvent`-free.
**Files.** `ue/Engine.*` (WorldToScreen + camera POV read), `features/Esp.h`.
**Size.** Medium. **Depends on:** confirming the `PlayerCameraManager` POV offset in-game.

### Spawn/despawn-diff actor cache

**Goal.** The `std::set_difference` approach: cache actor pointers frame-to-frame, `IsA`-classify only
newly-spawned actors, drop despawned ones — avoiding the per-frame `IsA` and, more importantly,
letting static objects (walls, pickups, portals) cache their unchanging data once on spawn.

**Approach.**
- In `cache/ActorCache`, keep last frame's pointer set; each frame build the current set, `set_difference`
  → `added` / `removed`. Classify `added` by `IsA` into typed lists (`players`, later `walls`, `pickups`,
  ...); erase `removed` from all. For static types, read position/size once at add time.
- Marginal for players here (`IsA` is a cheap hierarchy walk), so this lands *with* the first non-player
  feature that needs static-object data, not before. Combine with the location/team caching already in
  `ActorCache`.
**Files.** `cache/ActorCache.h`.
**Size.** Medium. **Depends on:** a feature that reads static world objects (to justify it).

---

## Visuals

### Glow / chams (per-team color + per-team toggle)

**Goal.** A team-colored glow around characters, with an independent enable + color for enemies and
teammates.

**Approach.**
- Splitgate already outlines players via stencil — the SDK shows `AlphaTeamStencilValue` /
  `BravoTeamStencilValue`. Reuse that path: for each cached character, `Mesh->SetRenderCustomDepth(true)`
  and set `Mesh->CustomDepthStencilValue` to a per-team value the game's (or a custom) post-process
  outline maps to a color. If the stock post-process doesn't expose arbitrary colors, a custom
  outline post-process material keyed on stencil value is needed (the harder part — needs in-game
  experimentation, no editor).
- Settings: `GlowEnemy` / `GlowFriendly` toggles + `GlowEnemyColor` / `GlowFriendColor`. Reset custom
  depth on the enable→disable edge (like other features' `Destroy`).
- New `features/Glow.h` driven from the shared `ActorCache` list; menu entries in Visuals.
**Files.** `features/Glow.h`, `features/Features.h`, `settings/Settings.h`, `menu/sections/Visuals.h`.
**Size.** Medium–large (the post-process color mapping is the risk). **Depends on:** in-game stencil
experimentation.

---

## Python scripting expansion

Today `scripting/` exposes `SplitgateInternal.{Logger, Settings, Events}`. These add powerful, and
dangerous, surface — everything runs on the game thread, actor pointers are valid only for the current
frame, and raw UE calls can crash the game, so each binding needs guard rails and clear docs.

### Read actors from scripts
**Goal.** `SplitgateInternal.Actors.players()` → a list of lightweight wrappers (`location`, `team`,
`name`, `health`, `is_local`). **Approach.** A pybind `Actors` submodule that reads from `ActorCache`
(so scripts share the one cached pass); wrappers hold plain values snapshotted for the frame, not raw
pointers, to keep scripts safe. **Files.** `scripting/modules/Actors.h`, `Scripts.h`, `docs/scripting.md`.

### Control / influence the local character
**Goal.** `SplitgateInternal.Player.teleport(x,y,z)`, `.location`, `.set_health(h)`, `.controller`.
**Approach.** A `Player` submodule wrapping the local character/controller UE calls (`K2_SetActorLocation`,
health field writes, `SendToConsole`). Guard every call on a valid, in-game controller. **Files.**
`scripting/modules/Player.h`, `Scripts.h`.

### Access UE utils from scripts
**Goal.** `SplitgateInternal.Engine.find_object(name)`, world/globals access, math helpers.
**Approach.** An `Engine` submodule exposing a curated, safe subset (object lookup by name, `FVector`
math, world/local-player accessors) — not the raw SDK. **Files.** `scripting/modules/Engine.h`.

### Draw from scripts
**Goal.** `SplitgateInternal.Render.line(a, b, color)`, `.text(pos, text, color)` from a per-frame
script hook. **Approach.** A `Render` submodule that forwards to the drawing layer (the feature-6
`Render` abstraction, so scripts get canvas or ImGui automatically). Requires exposing a per-frame
`Render` callback to scripts (a Python function invoked each frame, akin to the event bus). Coordinates
via the native/existing WorldToScreen. **Files.** `scripting/modules/Render.h`, `Scripts.h`, and the
render loop. **Depends on:** feature 6 (renderer) for the backend, ideally native WorldToScreen.

**Overall size (the four).** Large; best delivered as one "scripting API v2" pass with a documented,
safe surface and examples in `docs/scripting.md`.

---

## Events

### Pass the changed setting to `SettingsChanged`

**Goal.** `SettingsChanged` currently carries no payload, so every handler refreshes everything. Include
*which* setting changed so handlers (and scripts) can react selectively.

**Approach.** The event bus already supports `Events::Payload`; dispatch `SettingsChanged` with the
changed setting's identifier (a stable string key, e.g. `"VISUALS.Esp"`). The menu currently ORs a
single `changed` bool per tab — to name the culprit, either dispatch per-control with its key, or
group by section. Feature `UpdateEnabled` can then short-circuit when the key isn't theirs, and the
autosave/handlers stay as-is. Expose the payload to the Python `Events` module too.
**Files.** `scripting/Events.h`, every `menu/sections/*.h`, `features/*` (optional selective refresh),
`scripting/modules/Events.h`.
**Size.** Medium (touches every menu control). **Note:** mostly a menu-dispatch refactor.

---

## Engine SDK refactor (large)

**Goal.** `ue/Engine.h` is one enormous generated header plus `Engine.cpp` of hand-written
`ProcessEvent` wrappers, with free globals (`NamePoolData`, `ObjObjects`, `WRLD`, `GetBoneMatrixF`) and
`EngineInit` floating at namespace scope. Restructure into folders by concern, move the globals behind a
named namespace/class, and separate the reflection core from the game objects and their `ProcessEvent`
method bodies.

**Approach (incremental — build between each step).**
- `ue/math/` — `FVector`, `FVector2D`, `FRotator`, `FLinearColor`, `FMatrix`, `TArray`, `FString`.
- `ue/core/` — `FName`/`FNamePool`, `UObject`, `UClass`/`UStruct`, `TUObjectArray` (the reflection core;
  `IsA`, name lookup, `ProcessEvent`).
- `ue/objects/` — one header (struct) + cpp (`ProcessEvent` wrappers) per game type: `UEngine`,
  `UWorld`/`ULevel`, `APlayerController`, `APawn`/`ACharacter`, `USkeletalMeshComponent`, `UCanvas`, ...
- Globals → a `UE` namespace (or a small `Runtime` class): `UE::NamePool`, `UE::Objects`, `UE::World`,
  `UE::GetBoneMatrix`, `UE::Init()` — replacing `NamePoolData` / `ObjObjects` / `WRLD` / `GetBoneMatrixF`
  / `EngineInit` with clearer names and one place to see the resolved globals.
- Keep each type's `ProcessEvent` wrappers next to the type, not in one giant `Engine.cpp`.

**Caveats.**
- Much of `Engine.h` is *generated* from the Dumpspace dump (see [game-dump.md](game-dump.md)); a split
  must preserve the cross-references between structs and stay regenerable, or the generator/inputs get
  updated to emit the new layout.
- The forbidden files reference some of these names (e.g. `dllmain.cpp` via the init flow). Renaming
  `EngineInit`/globals means a `TODO.md` entry for the forbidden side, done last.
- Do it in small, buildable steps (math → core → objects → globals), never one commit.
**Files.** All of `ue/`, plus every include site; `TODO.md` for the forbidden references.
**Size.** XL, highest risk of everything here. Sequence it after the perf/scripting work so it isn't
blocking features.

---

## GUI

### Recent logs panel

**Goal.** View recent log lines in the overlay instead of only the console/`internal.log`.

**Approach.** Give `Shared::Logger` a small ring buffer of recent formatted lines (mirroring the
existing `Network::Http::Recent()` pattern) with a `Recent()` accessor, and render it in a "Logs" tab
(or a collapsible in Debug) with a clear button and level coloring — the same shape as the Network
tab's "Request flow" panel.
**Files.** `shared/Logger.h` (ring buffer + `Recent()`), a `menu/sections/Logs.h` (or an addition to
`Debug.h`), `menu/Menu.h` if a new tab.
**Size.** Small–medium.

---

## Suggested sequencing

1. **Native WorldToScreen** — biggest standalone perf win, unblocks cheaper drawing everywhere.
2. **Recent logs panel** + **`SettingsChanged` payload** — small, high quality-of-life.
3. **Glow** — self-contained visual feature.
4. **Scripting API v2** (read actors → player control → UE utils → draw) — one themed pass; draw depends
   on the renderer (feature 6) and native WorldToScreen.
5. **Spawn/despawn-diff cache** — with the first static-object feature.
6. **Engine SDK refactor** — last; large and touchy, do it when features aren't in flight.
