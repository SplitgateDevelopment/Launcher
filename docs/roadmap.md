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

## Cameras

### Custom third-person (and free-cam), since the game forces first person

**Why the mode strings don't work.** `ClientSetCameraMode("ThirdPerson")` never engaged because
Splitgate's camera manager (a custom `APortalWarsPlayerCameraManager`) forces first person and
ignores the style. The free camera only worked via the `ToggleDebugCamera` console command (a
built-in UE debug spectator), which is why `FreeCam` now uses `SendToConsole`.

**How to discover camera modes when ProcessEvent shows nothing.** The game suppresses the mode
change, so it never dispatches — you can't log it. Instead:
- Read `APlayerCameraManager::CameraStyle` (an `FName` field) at runtime to see the current style,
  and enumerate camera-related `FName`s from the GNames dump (see [game-dump.md](game-dump.md)).
- Watch ProcessEvent for the camera *update* path while in the debug camera (`UpdateCamera` /
  `DoUpdateCamera` / the view target's `CalcCamera`) — those *do* run.

**Custom third person (yes, we can build our own).** Don't rely on a mode string; take over the
final view:
- Hook the camera POV computation — the pawn's/`PlayerCameraManager`'s `CalcCamera` /
  `DoUpdateCamera` (find it via the camera-manager vtable or an AOB; it may be native, so a MinHook
  by-signature hook rather than a ProcessEvent one). In the hook, offset the out
  `FMinimalViewInfo.Location` backward from the player: `loc -= forward * Distance; loc += up *
  Height;` with a wall trace to pull in on collision. Configurable distance/height/side.
- Start here: a `ThirdPerson` rewrite that hooks the camera update and applies a fixed offset, then
  add collision + tuning. **Size:** Large (needs the camera-update function via RE). Free-cam via
  `ToggleDebugCamera` already works as the interim.

## Renderers (beyond canvas / ImGui)

The `Render::Backends[]` registry makes new backends a new enum value + one array entry. Worth
adding:
- **Null renderer** — draws nothing; a baseline for measuring the ProcessEvent/ImGui cost, and a
  quick global "hide overlays".
- **External overlay renderer** — draws into a separate, capture-excluded window (the streamproof
  fix below). It's the natural third backend: `RendererMode::ImGuiStreamproof`.

### Streamproof via a separate excluded overlay window

**Why the current one is wrong.** `WDA_EXCLUDEFROMCAPTURE` on the *game* window hides the whole
game from capture (whole-screen capture included). To hide *only* the overlay it must live in its
own window.

**Approach.** Create a layered, transparent, click-through, top-most window sized to the game
(`WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW`, `SetLayeredWindowAttributes`
or a DWM extend for transparency), set `WDA_EXCLUDEFROMCAPTURE` on it, give it its own D3D11
device+swapchain and a second ImGui context. The ImGui renderer's `Flush()` targets this window's
draw list when streamproof is on; drive its Present from the game's Present hook and keep its rect
synced to the game window. **Input caveat:** click-through means the *menu* isn't interactive there
— keep the menu on the game window (visible in your own view, not to capture is impossible for an
interactive window), and route only the ESP/drawings to the excluded window. **Size:** Large; a
full second render pipeline. This is why it's planned, not done — it's not a small toggle.

## Aim

### True (trace-redirect) silent aim

The current silent aim snaps the view on fire. A truly invisible one redirects the *shot*, not the
view: **firing is not suppressed like the camera, so the fire UFunction shows up in ProcessEvent** —
log it, then hook it (via the event bus or MinHook) and rewrite the trace start/direction (or the
hit result) toward the selected target before the original runs, leaving `ControlRotation`
untouched. **Size:** Medium; needs the fire function name (discoverable in-game).

## Requested UI / QoL

### Unload button
A GUI button (Debug or Misc) that triggers `Hook::UnHook` on a detached thread (the same teardown
the Shutdown event runs), so the DLL can be unloaded on demand. **Size:** Small.

### Fix "Summon Bot" / actor spawn
`SendToConsole("summon PortalWarsBot_BP_C")` doesn't spawn — likely the wrong class path or a
blocked `summon`. Confirm the bot's full class name from the GObjects dump (**Debug → Dump
GObjects**) and fix the command (and/or use `SpawnObject`/`SpawnActor` with the resolved class).
**Size:** Small (needs the class name).

### RGB for everything colorable
A `Color` (with an ImGui `ColorEdit4`) for every drawable/tintable element, unified in one place:
ESP lines/boxes/bones/name/health (partly done), radar self-icon, watermark text, the ImGui menu
accent / top-bar (via `ImGuiStyle` colors), a custom crosshair, and the render-side ones — mesh /
chams / glow, and (if achievable) our gun and player. Approach: extend the relevant settings with
`Color` fields and wire each draw/style; the menu ones set `ImGui::GetStyle().Colors[...]`; the
mesh/gun/player ones ride on the glow/cosmetics work. **Size:** Medium spread (menu/watermark are
easy; mesh/gun/player depend on glow/cosmetics).

### Cosmetics changer (player / gun / emotes)
Override the local loadout's skins/materials/emote ids on the character and weapon. Caveat:
cosmetics are likely server-authoritative, so changes may be **client-visual only or revert** —
scope it as a visual override and verify in-game. Needs the skin/material/emote fields via RE.
**Size:** Large.

### Announce toggles in chat (from the feature, not the player)
When enabled and in a game, post `[ESP] Enabled` / `[ESP] 3D boxes on` on each toggle. Key point:
`PlayerController->SendChatMessage` sends to the **server** (everyone sees it as you) — wrong for
this. Use a **client-only local message** instead (e.g. `APlayerController::ClientMessage`, or the
chat widget's local "add message"), so it shows only in your chat, labeled by the feature. This
pairs with the **`SettingsChanged` payload** item (to know *which* setting/feature changed).
**Size:** Medium; needs a local-message function + the changed-setting payload.

## Integrations

### Discord Rich Presence tab + richer state
A dedicated Discord tab to configure presence, and more state: track kills (from the `PlayerKilled`
event / `PlayerState` score), show the current level/map and elapsed game time, party/mode, etc.,
updating the RPC `details`/`state`/timestamps periodically. Builds on the existing `discord/` RPC.
**Size:** Medium (game-state reads + RPC fields + a menu tab).

## Suggested sequencing

1. **Native WorldToScreen** — biggest standalone perf win, unblocks cheaper drawing everywhere.
2. **Recent logs panel** + **`SettingsChanged` payload** — small, high quality-of-life.
3. **Glow** — self-contained visual feature.
4. **Scripting API v2** (read actors → player control → UE utils → draw) — one themed pass; draw depends
   on the renderer (feature 6) and native WorldToScreen.
5. **Spawn/despawn-diff cache** — with the first static-object feature.
6. **Engine SDK refactor** — last; large and touchy, do it when features aren't in flight.
