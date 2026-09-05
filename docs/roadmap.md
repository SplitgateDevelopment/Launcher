# Roadmap

Design notes for larger future work, beyond the near-term [planned-features.md](planned-features.md)
(minidumps, streamproof, guard-hook, aimbot, profiles, renderer). Most entries are the plan; the ones
marked **DONE** / **PARTIAL** have been built (kept for reference). Grouped by theme; each entry has an
approach, the files it touches, a rough size, and dependencies. "Forbidden files" =
`Internal/dllmain.cpp` / `Launcher/Launcher.cpp`, whose edits go to `TODO.md`.

**Still outstanding (need RE / in-game / a separate render pipeline, so not done blind):** the
streamproof separate-overlay window and the custom third-person camera. The glow / chams shipped
(built on the game's own team-outline post-process; the arbitrary-color mapping still wants an
in-game check). The spawn/despawn-diff cache was analysed and intentionally skipped — see its note.

---

## Performance

### Native WorldToScreen — DONE

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

### Spawn/despawn-diff actor cache — SKIPPED (analysed, not beneficial yet)

> **Update:** measured against reality, this doesn't help here and could hurt. `IsA` is a handful of
> pointer compares, and the cache must still re-read each player's location/team every frame (they
> move), so the diff only avoids the cheap `IsA` on unchanged actors — while *adding* the cost of
> building an unordered_set of every actor pointer each frame plus the set difference. It only pays
> off with expensive classification or cached **static** data (walls/pickups), so it should land with
> the first such feature, not now. Kept as a plan.

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

### Offset-only reads (drop the remaining ProcessEvent/AOB calls)

**Goal.** The ESP/aim still lean on a few `ProcessEvent` UFunctions (`K2_GetActorLocation`,
`GetTeamNum`) and an AOB-scanned `GetBoneMatrix`. The UE4 memory layout exposes the same data as
plain field reads, which the [ue4-cheatsheet.md](ue4-cheatsheet.md) collects — swapping to them
removes per-actor/per-bone call cost entirely.

**Approach (incremental, confirm each offset against the dump first).**
- **Location — DONE:** `utils/ActorLocation.h::ActorLocation()` reads
  `RootComponent->RelativeLocation` (0x130 → 0x11c), used by `ActorCache`; falls back to
  `K2_GetActorLocation` when the root component is missing or the **Native actor location** Debug
  toggle is off.
- **Bones:** read the bone array directly (`USkeletalMeshComponent->LODData - 0x4` on UE4; `- 0x8`
  and `double` `FMatrix` on UE5) rather than the AOB `GetBoneMatrix` — no signature to maintain and no
  call per bone. Keep the AOB path as a fallback (a Debug toggle, like the existing native/UFunction
  ones).
- **Visibility — DONE:** `utils/Visibility.h::IsVisible()` reads `LastRenderTimeOnScreen` (0x290) vs
  `LastSubmitTime` (0x288) off the mesh — no ProcessEvent — shared by the ESP visibility recolor and
  the aimbot/triggerbot visible check; falls back to `WasRecentlyRendered` behind the Debug **Native
  visibility** toggle.
- **Actor id:** `Actor + 0x18` for a stable per-actor key (useful for the spawn/despawn-diff cache and
  any per-actor state).
**Files.** `ue/Engine.*` (offset accessors), `cache/ActorCache.h`, `features/Esp.h` /
`features/Aimbot.h`, `settings/Settings.h` + `menu/sections/Debug.h` (fallback toggles).
**Size.** Medium, spread across small verified steps. **Depends on:** confirming each offset in-game /
against [game-dump.md](game-dump.md).

---

## Visuals

### Glow / chams (per-team color + per-team toggle) — DONE (in-game color mapping still to verify)

**Goal.** A team-colored glow around characters, with an independent enable + color for enemies and
teammates.

**Shipped as** [`features/Glow.h`](../Internal/features/Glow.h): the approach below was built as
described — `Mesh->SetRenderCustomDepth(true)` + `CustomDepthStencilValue` from the character's own
`Enemy`/`FriendlyStencilValue`, colors written into the character's `Blue`/`RedOutlineColor` (both),
per-team toggle + color, RGB rainbow override, `Destroy()` resetting custom depth on the still-cached
meshes. `UPrimitiveComponent::SetRenderCustomDepth` is wrapped in the SDK; `ActorCache` rebuilds when
a glow toggle is on. **Remaining:** confirm in-game whether the stock post-process honors an arbitrary
color (else a custom outline material keyed on stencil value is needed).

**Dump-confirmed hooks:** `Function Engine.PrimitiveComponent.SetRenderCustomDepth`,
`SetCustomDepthStencilValue`, `SetCustomDepthStencilWriteMask`, and `Enum Engine.ECustomDepthStencil`
all exist — so the stencil path below is wrappable. The remaining risk stays the post-process color
mapping (whether the stock outline exposes arbitrary colors), which needs in-game experimentation.

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

## Python scripting expansion — DONE (scripting API v2)

`scripting/` now exposes `SplitgateInternal.{Logger, Settings, Events, Actors, Player, Engine,
Render, Input}`. Everything runs on the game thread (script `main()` from the render loop, and
`Events` handlers from their dispatch site), so game calls are safe; snapshots hold plain values, not
raw pointers, to stay frame-safe. Documented with examples in [scripting.md](scripting.md).

### Read actors from scripts — DONE
`SplitgateInternal.Actors.players()` / `.enemies()` / `.count()` → snapshot `Player` objects
(`x/y/z`, `location`, `team`, `health`, `max_health`, `name`, `is_local`, `address`). Rebuilds the
shared `ActorCache` unconditionally so scripts see actors regardless of the ESP/aim toggles.
**Files.** `scripting/modules/Actors.h`, `cache/ActorCache.h` (`Rebuild()`), `Scripts.h`.

### Control / influence the local character — DONE
`SplitgateInternal.Player.{is_in_game, location, teleport(x,y,z), health, set_health, view_rotation,
set_view_rotation, console(cmd), chat(msg)}`. Wraps the local character/controller UE calls
(`K2_TeleportTo`, the `Health` field, `ControlRotation`/`SetControlRotation`, `SendToConsole`,
`SendChatMessage`), each guarded on a valid controller/pawn. **Files.** `scripting/modules/Player.h`.

### Access UE utils from scripts — DONE
`SplitgateInternal.Engine.{find_object(name), world_to_screen(x,y,z), canvas_size(), distance(...)}`
— a curated safe subset (object lookup, native projection, viewport size, cm→m distance), not the raw
SDK. **Files.** `scripting/modules/Engine.h`.

### Draw from scripts — DONE
`SplitgateInternal.Render.{line, text, circle, world_line, world_text}` forward to the `Render`
abstraction (canvas/ImGui automatically) and project via the native WorldToScreen. Call them from a
handler subscribed to `Events.Render` (already dispatched each frame by the UserScripts feature).
**Files.** `scripting/modules/Render.h`.

### Extras shipped alongside
- **`Input`** — `is_key_down(vk)` / `is_key_pressed(vk)` so scripts can gate on a key.
- **Generic settings bridge** — `Settings.get(path)` / `set(path, value)` / `toggle(path)` with
  dotted paths (e.g. `"VISUALS.Esp"`, `"AIM.AimFov"`, `"MENU.Rgb"`), JSON-backed off the same
  serialization the config uses, so it covers **every** setting — current and future — for free, and
  dispatches `SettingsChanged` like the menu. This is how scripts drive the program's latest features
  (RGB, aim visibility check, FOV circle, SSL bypass, ...).

### Further ideas — DONE (this pass)
- **`Actors` extras — DONE:** `player.bone(Actors.Bone.*)` world positions, `player.visible()` (via
  `WasRecentlyRendered`), `player.distance()` to local; an `Actors.Bone` enum. (Weapon/loadout fields
  still need RE.)
- **`Player` extras — DONE:** `velocity()`, `aim_at(x,y,z)`. (Ammo/loadout and `respawn()` still need
  RE / the fire path.)
- **`Game` module — DONE:** `fps()`, `map_name()`, `local_name()`. (Mode/score/round/timer need RE —
  some are on the event bus already.)
- **`Render` extras — DONE:** `rect()` outline and `skeleton(player)` one-call ESP. (Filled rect /
  gradient need a backend fill primitive.)
- **`Events` extras — DONE:** script-to-script custom events (`emit(name, value)` /
  `on_custom(name, cb)`). More *game* events still depend on the fire/trace RE.
- **Hot-reload — DONE:** `Scripts::Reload()` + a **Reload** button and per-script **Run** in the
  **Scripts** tab (re-imports via `importlib.reload`). Script-registered event handlers (bus and
  custom) are cleared before re-importing, so reloading never stacks duplicates.

### Still-open scripting ideas
- Weapon/loadout/ammo reads and `respawn()`/`suicide()` (need the weapon/loadout RE + fire path).
- A richer `Game` module (mode, score, round state, match timer) once those reads are located.
- ~~Filled-rect / circle / gradient draw~~ **DONE** — `Render::RectFilled` / `CircleFilled` /
  `RectGradient` (ImGui `AddRectFilled` / `AddCircleFilled` / `AddRectFilledMultiColor`; canvas
  scanline/interpolated approximations) + `rect_filled` / `circle_filled` / `rect_gradient` in scripts.
- More *game* events (weapon fire, portal spawned, pickup) as the fire/trace hook lands.
- ~~Scoped script-handler unregister so hot-reload doesn't stack bus-event subscriptions.~~ **DONE** —
  `Events::Register` returns an id and `Events::Unregister(id)` removes it; the Events module tracks
  script-registered bus-handler ids and `Scripts::Reload()` clears them (and the custom-event ones).

---

## Events

### Pass the changed setting to `SettingsChanged` — DONE (infrastructure + Exploits tab)

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

### Recent logs panel — DONE

**Goal.** View recent log lines in the overlay instead of only the console/`internal.log`.

**Approach.** Give `Shared::Logger` a small ring buffer of recent formatted lines (mirroring the
existing `Network::Http::Recent()` pattern) with a `Recent()` accessor, and render it in a "Logs" tab
(or a collapsible in Debug) with a clear button and level coloring — the same shape as the Network
tab's "Request flow" panel.
**Files.** `shared/Logger.h` (ring buffer + `Recent()`), a `menu/sections/Logs.h` (or an addition to
`Debug.h`), `menu/Menu.h` if a new tab.
**Size.** Small–medium.

---

### Realtime SDK viewer — DONE

Shipped as a dedicated **SDK tab** ([menu/sections/Sdk.h](../Internal/menu/sections/Sdk.h)):
**Object search** (scan every GObject, list those whose full name contains the text), **Class search**
(filter the cached list of every class; click a row to copy the exact name), **Class instances**
(resolve a class by name — trying `Class …` / `BlueprintGeneratedClass …` forms — then list its live
instances via `IsA`, with addresses), and the **GObjects dump** (moved here from Debug). The class list
is a shared, lazily-built [ClassCache](../Internal/cache/ClassCache.h) (identifies classes by their
`ClassPrivate` meta-class, so `GetFullName` is only paid on classes) reused by the **Misc spawn
picker**. Result lists are clipped to visible rows; scans run on button press.

**Original goal.** An in-overlay explorer of the live UObject world: search for a class by name and
list its valid instances, enumerate `UClass`es, and browse `GObjects`.

**Approach.** Everything is already reachable: `ObjObjects` (the `TUObjectArray`) is iterated in
**Debug → Dump GObjects**, `ObjObjects->FindObject(name)` resolves by full name, and `UObject`
exposes `GetFullName`/`GetName`/`IsA`. Build a `menu/sections/Sdk.h` tab:
- **Class search:** an input box → `FindObject("Class <...>")`; then one `GObjects` pass collecting
  every object whose class `IsA` the searched class → list them (index, full name, address), with a
  filter and a cap so a broad class doesn't flood the UI.
- **Enumerate UClasses:** one `GObjects` pass listing objects whose class is `Class` (i.e. the
  `UClass` instances), searchable — the live equivalent of the dump.
- **Object browser:** page through `GObjects` (index range) with the name filter, showing
  `GetFullName()`; clicking one could later show fields (needs the dumped struct layout to be useful).
Do the scans on demand (button / throttled), not every frame — a full `GObjects` walk is ~the Dump
GObjects cost. **Files.** `menu/sections/Sdk.h`, `menu/Menu.h` (new tab); reuses `ue/UObjects.h`.
**Size.** Medium; a self-contained RE tool that pairs well with the [ue4-cheatsheet.md](ue4-cheatsheet.md).

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

### True (trace-redirect) silent aim — BLOCKED (needs the native fire/trace function)

The current silent aim snaps the view on fire. A truly invisible one redirects the *shot*, not the
view: hook the fire/trace path and rewrite the trace start/direction (or the hit result) toward the
selected target before the original runs, leaving `ControlRotation` untouched.

**Why it isn't built yet.** The SDK dump exposes no clean hook point: the shot's trace is computed in
the `Gun`'s **native** fire code, and the plausible reflected sources (`APlayerController::StartFire`,
`APawn::GetBaseAimRotation`) are called *natively*, so the `ProcessEvent` hook — which only sees
reflected/Blueprint calls (as `EnableAllInput` does) — never observes them. (A dump search confirms
this: the only reflected `Gun` fire-path entry is `Function PortalWars.Gun.ServerGoToState`, no
`Fire`/`HitScan`/`ProcessHit` UFunction.) Redirecting the shot therefore needs a **MinHook on the
native function**, which can't be found/verified blind.

**Next step (one in-game pass unblocks it).** Enable **Debug → Log ProcessEvent** and fire: if *any*
reflected fire/hit event appears (e.g. a `Server*Fire` / `ProcessHit` / weapon-fire UFunction), hook
it via the event bus / a `ProcessEvent` intercept and rewrite its trace params. If nothing reflected
shows, RE the `Gun` native fire (an AOB like the `curl_easy_setopt` one in
[ue4-cheatsheet.md](ue4-cheatsheet.md)) and MinHook it. Then a `bool TrueSilentAim` gates the rewrite.
**Files.** the fire hook, `settings/Settings.h`, `menu/sections/Aim.h`. **Size:** Medium; **depends
on:** identifying the fire/trace function in-game.

### Phasing bullets (wallbang) toggle — Approach A DONE

**Goal.** Let the local player's shots register through world geometry, so a target behind cover
can still be hit.

**Shipped (Approach A):** [`features/Phasing.h`](../Internal/features/Phasing.h) walks the actor list
and `SetActorEnableCollision(false)` on every `CullableActor` (cover), scoped so the floor/BSP keeps
colliding and `ABaseGun` pickups are skipped; rescans every 30 frames; `Destroy()` re-enables via a
live actor walk. Toggle in Exploits > Weapon. Server may still validate hits (private-backend best);
verify in-game. Approach B (native trace redirect) stays blocked on the fire function.

**Approach A — disable wall collision (no native hook needed, worth trying first).** Cycle the actor
list (like `ActorCache`), and for every world/geometry actor call
`SetActorEnableCollision(false)` (`Function Engine.Actor.SetActorEnableCollision` — a ProcessEvent
wrapper like the others in `Engine.cpp`). With the walls non-colliding, a hitscan trace passes through
them. Re-enable on toggle-off (cache the set we disabled, or just re-enable all). Caveats: identifying
"a wall" (filter by class — `StaticMeshActor`/`BSP`/`CullableActor`, not players/pickups); it may
break local movement/physics (you'd fall through floors) so scope it to walls not floors, or gate it
to only-while-firing; and the server may still validate the hit. Cheapest path — try it before the
native hook.

**Approach B — native fire/trace hook (see the trace-redirect silent aim above).** Rewrite the shot's
trace collision channel / query params to ignore `WorldStatic`, or force the hit result onto the
target. Needs the native fire function (BLOCKED on the same in-game RE), but is the "clean" version and
doesn't disturb world physics.

Guarded on a `bool PhasingBullets`. **Files.** `ue/Engine.*` (`SetActorEnableCollision` wrapper),
`settings/Settings.h`, a feature, `menu/sections/Aim.h` (or Exploits). **Size.** Medium.
**Depends on:** (A) a wall-class filter + in-game testing that it doesn't break movement; (B) the
fire/trace function.

### Aimbot visibility check toggle — DONE

**Goal.** Only lock onto targets in line of sight, so the aimbot ignores enemies behind walls.

**Shipped.** A `bool AimVisibleCheck` (`AimSettings`, Aim tab) that skips any candidate the game
reports as not recently rendered, in both the aimbot and triggerbot target passes. Uses the game's own
`AActor::WasRecentlyRendered(0.1f)` (a wrapper added to `Engine.cpp`) — occlusion-aware and correct,
one `ProcessEvent` per contended candidate while aiming. Preferred over the render-flag offset trick
because the dump leaves `LastRenderTimeOnScreen`/`LastSubmitTime` in a padded region (uncertain
offset); if a per-frame, ProcessEvent-free version is ever wanted, those offsets off
`UPrimitiveComponent->BoundsScale` (0x284) are the path (see
[ue4-cheatsheet.md](ue4-cheatsheet.md#offsets-you-derive-from-a-neighbour)).

### Per-bone visibility check (LineTraceSingle) — DONE (trace channel to verify in-game)

**Shipped:** `LineTraceVisible()` wraps `UKismetSystemLibrary::LineTraceSingle` in
[Engine.cpp](../Internal/ue/Engine.cpp) (param block laid out to the engine's offsets, guarded by
`static_assert`s, fails open), and the aimbot gained a **Per-bone visibility** strict sub-mode under
the visibility check ([Aimbot.h](../Internal/features/Aimbot.h)): it traces the eye→bone ray ignoring
the target and aims at the first bone in line of sight (configured bone → head → chest → pelvis),
skipping targets with no visible bone; only traced while aiming. **Remaining:** trace channel 0
(Visibility) may need an in-game check if the game remaps it. A companion ESP "visibility check"
(whole-actor recolor via `WasRecentlyRendered`) also shipped in [Esp.h](../Internal/features/Esp.h).

**Confirmed in the dump:** `Function Engine.KismetSystemLibrary.LineTraceSingle` (and
`LineTraceSingleByChannel` / `...ByProfile` / `...ForObjects`) exist — so the trace wrapper is
buildable; only the exact visibility channel/profile still needs a quick in-game check.

**Goal.** A stricter visibility test than the current whole-actor `WasRecentlyRendered` — decide
per *bone* whether that exact point is in line of sight, so the aimbot can pick a visible bone (e.g.
skip the head when only the feet are exposed) instead of accepting/rejecting the whole target.

**Approach.** For a candidate bone's world position, trace from the camera (`CameraCachePrivate.POV`,
already read by [WorldToScreen.h](../Internal/utils/WorldToScreen.h)) to the bone with
`UKismetSystemLibrary::LineTraceSingle` (or `UWorld::LineTraceSingleByChannel`) on the visibility
channel, ignoring the local pawn; the bone is visible if there's no blocking hit before it (or the hit
actor is the target). Wrap the trace UFunction in `Engine.cpp`. In the aimbot, prefer the configured
bone, else fall back to the first visible bone in a priority list (head → chest → pelvis). One
`ProcessEvent` per tested bone, so gate it behind `AimVisibleCheck`'s "strict" sub-mode and only test
until a visible bone is found. **Files.** `ue/Engine.*` (trace wrapper), `features/Aimbot.h`,
`settings/Settings.h`, `menu/sections/Aim.h`. **Size.** Medium. **Depends on:** a `LineTraceSingle`
wrapper + confirming the visibility trace channel in-game.

### Draw aim FOV circle — DONE

**Goal.** Optionally draw a circle at the crosshair with radius = `AimFov`, so the lock-on cone is
visible while tuning.

**Shipped.** A `bool DrawAimFov` + `Color AimFovColor` (`AimSettings`, Aim tab) and a small render
feature (`features/AimFov.h`) that draws a 48-segment circle of radius `AimFov` px at screen centre
through the `Render` abstraction, so it follows whichever renderer is active.

## Projectiles

### Bullet speed / "bullet TP"

**Goal.** Change how fast the local player's projectiles travel — slow them down to watch, or crank the
speed so they hit near-instantly ("bullet teleport").

**Approach.** Projectiles are actors in the world, so cycle the actor list (like `ActorCache`), find
the one(s) that are bullets — filter by class (the projectile `UClass`, resolved from the GObjects
dump; e.g. a `PortalWars.*Projectile`/`Gun`-spawned actor) and optionally "owned by the local pawn" —
and modify their movement each frame. Two knobs: scale the actor's velocity
(`AActor` velocity / the `UProjectileMovementComponent`'s `Velocity` + `InitialSpeed`/`MaxSpeed`), or
push the actor along its velocity toward the target (the "TP" effect) by writing its location. Both are
field writes once the projectile actor + its movement component are located; confirm the offsets from
the dump. Guarded on a setting with a speed multiplier. Caveat: projectile motion is often
server-simulated, so this may be client-visual only — verify in-game. **Files.** `cache/ActorCache.h`
(or a projectile pass), a `features/BulletSpeed.h`, `settings/Settings.h`, `menu/sections/Exploits.h`.
**Size.** Medium. **Depends on:** the projectile class + movement-component offsets.

### Bullet traces (tracer trails) — DONE (projectile weapons)

Shipped as `features/BulletTraces.h` (Visuals tab, `VISUALS.BulletTraces` + duration + color,
RGB-aware). Each frame it finds live projectile actors (`IsA Class PortalWars.Projectile`, which
covers Explosive/EMP/Impact/Plasma), records a timestamped world position per projectile, and draws
fading connecting lines via `Projection::WorldToScreen` + `Render::Line`. Trails are only drawn from
recorded positions, so a despawned projectile's trail lingers and fades without dereferencing the dead
actor. **Still open:** hitscan weapons (no projectile actor) — that needs the fire
trace's start/end — which ties into the native fire hook (BLOCKED item above); the projectile path is
independent and is what's shipped.

## Requested UI / QoL

### Unload button — DONE
A GUI button (Debug or Misc) that triggers `Hook::UnHook` on a detached thread (the same teardown
the Shutdown event runs), so the DLL can be unloaded on demand. **Size:** Small.

### Fix "Summon Bot" / actor spawn — DONE (mechanism; needs the real class name in-game)
The deferred-spawn machinery is built: `FQuat`/`FTransform`/`ESpawnActorCollisionHandlingMethod` +
`UGameplayStatics::BeginDeferredActorSpawnFromClass` / `FinishSpawningActor` wrappers and a
`SpawnActor(worldContext, class, location, collision, owner)` helper in `ue/Engine.*`. The **Summon
Bot** button (moved to the **Misc** tab) resolves the bot class by name, calls `SpawnActor` in front
of the local pawn, and **falls back to the console `summon`** if the class doesn't resolve. The one
remaining unknown is the bot's exact class name — find it via the new **SDK tab** (search "Bot") and
drop it into the button. Param-struct layouts follow UE4 (FTransform 16-aligned at 0x10); verify
in-game.

**Proper spawn via the deferred two-step (`UGameplayStatics`).** Instead of the console `summon`,
spawn the actor directly through `UGameplayStatics` — the reliable pattern is
`BeginDeferredActorSpawnFromClass` → set any pre-spawn state → `FinishSpawningActor`
([SpawnActorDeferred](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/Engine/UWorld/SpawnActorDeferred?application_version=4.27),
[BeginDeferredActorSpawnFromClass](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/Kismet/UGameplayStatics/BeginDeferredActorSpawnFromClass?application_version=4.27),
[FinishSpawningActor](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/Kismet/UGameplayStatics/FinishSpawningActor?application_version=4.27)).
Both are `UGameplayStatics` UFunctions, so wrap them like the existing `Engine.cpp` `ProcessEvent`
wrappers, called on the `UGameplayStatics` CDO (`Globals::GameplayStatics`, already resolved). Sketch:

```cpp
// on UGameplayStatics: STATIC_BeginDeferredActorSpawnFromClass / STATIC_FinishSpawningActor
AActor* SpawnActor(UObject* worldContext, UClass* actorClass, FVector loc,
                   ESpawnActorCollisionHandlingMethod collision, AActor* owner)
{
    FTransform t;
    t.Translation = loc;
    t.Scale3D = FVector{1, 1, 1};
    t.Rotation = FQuat{0, 0, 0, 1};

    AActor* deferred = GameplayStatics->BeginDeferredActorSpawnFromClass(worldContext, actorClass, t, collision, owner);
    if (!deferred) return nullptr;
    return GameplayStatics->FinishSpawningActor(deferred, t); // set pre-spawn state between the two if needed
}
```

`worldContext` = the world/`PlayerController`; `actorClass` = the resolved bot `UClass` (from the
GObjects dump); `collision` = e.g. `AdjustIfPossibleButAlwaysSpawn`. This avoids the console entirely
and is the canonical way to spawn Blueprint actors. **Files.** `ue/Engine.*` (two wrappers +
`FTransform`/`FQuat`/`ESpawnActorCollisionHandlingMethod` in the SDK), `menu/sections/Debug.h` (the
button), a resolved bot class name. **Size:** Small–medium (needs the class name + the SDK types).

### RGB for everything colorable — PARTIAL (watermark, menu accent, radar self done)
Two strands. **(1) Per-element pickers:** a `Color` (ImGui `ColorEdit4`) for every drawable element —
ESP lines/boxes/bones/name/health (done), a **custom crosshair (done** — `features/Crosshair.h`, a
size/gap/thickness/color cross at screen centre, RGB-aware, in the Visuals tab), and the render-side
ones (mesh / chams / glow, and if achievable our gun and player, riding on the glow/cosmetics work). **(2) A single RGB
rainbow toggle** (`MenuSettings.Rgb`, done): a time-cycled hue (`utils/Rgb.h`) that tints the
watermark, the ImGui menu accent (`ImGui::GetStyle().Colors[...]`, snapshotting the theme defaults so
toggling off restores them), and the radar self-icon; off falls back to their defaults (red / white)
rather than a user-picked color. Future colorable elements pick a strand: static ones get a picker,
menu chrome joins the rainbow. **Size:** Medium spread (menu/watermark/rainbow done;
mesh/gun/player depend on glow/cosmetics).

### Cosmetics changer (player / gun / emotes)
Override the local loadout's skins/materials/emote ids on the character and weapon. Caveat:
cosmetics are likely server-authoritative, so changes may be **client-visual only or revert** —
scope it as a visual override and verify in-game. Needs the skin/material/emote fields via RE.
**Size:** Large.

### Announce toggles in chat (from the feature, not the player) — DONE
`Settings.MISC.AnnounceToggles` (Misc → Program). The `AnnounceToggles` feature runs on
`SettingsChanged`, diffs a snapshot of the tracked feature bools (ESP, radar, aimbot, triggerbot, god
mode, jetpack, no-recoil, spin-bot, third-person, free-cam, infinite-ammo, no-reload), and on a real
change posts `[ESP] Enabled` / `Disabled` via **`APlayerController::ClientMessage`** — a client-only
local message (shown only to you, **not** sent to the server, unlike `SendChatMessage`). It snapshots
without announcing on enable and re-seeds on disable, so it never spams. Independent of the
`SettingsChanged` payload (it diffs state itself), so it works for every tracked toggle.

## Integrations

### Discord Rich Presence tab + richer state — DONE
- **Richer state:** the `DiscordPresence` feature refreshes the RPC `state` every ~5s from live game
  reads — the current map (`World->GetName()`) and the local player's K/D
  (`APortalWarsPlayerState.PlayerStats.Kills/Deaths`), or "In menu" out of a match
  (`DiscordRPC::UpdateGameState` in `discord/rpc.h`). Cheap field reads, no ProcessEvent, on change.
- **Tab:** a dedicated **Discord** tab ([menu/sections/Discord.h](../Internal/menu/sections/Discord.h))
  — enable/disable presence (clears it from your profile when off), the live state/details, a
  **Refresh now** button, and the app/image info.
- **Still open (nice-to-have):** elapsed game time / party / mode fields.

## Tooling (reverse-engineering)

### Signature scripts — DONE (offline) + known-signatures registry

`Tools/find_signature.py` locates a function and emits a masked AOB (callsite / bytes / stringref
strategies; masks rel32 + rip-relative with `0x00`, the project's wildcard). `Tools/known_signatures.py`
runs every signature the DLL relies on (currently `curl_easy_setopt` and `GetBoneMatrix`) in one pass,
so re-deriving after a game update is one command. Keep new AOBs in that registry.

### memcury-style AOB engine + string-ref discovery (runtime) — DONE (engine); wiring optional

**Shipped:** `utils/Memcury.h` — IDA `??`-pattern parsing + wildcard `FindPattern`, a `Scanner` with
`RelativeOffset` (follow a `lea`/`call` disp32 to its target) and `GetAs<T>`, module helpers (`Scan`),
and `FindStringRef` (string → the `lea` that references it). The pure core is unit-tested against
crafted buffers in `Tests/MemcuryTests.cpp` (6 tests). **Still optional:** swap `CurlHook` /
`GetBoneMatrix` resolution onto it (keeping `FindSignature` as the fallback) — the engine is ready;
this is just a low-risk rewire.

**Goal.** The runtime scanner (`Util.cpp::FindSignature`) uses **`0x00` as the wildcard**, so any real
`0x00` byte in a pattern silently becomes a wildcard — fragile, and it can't follow relative
instructions. Adopt a small [memcury](https://github.com/projectnovafn/Sinum/blob/main/Windows/Utilities/memcury.h)-style
API instead (single header, MIT):

- **IDA-style patterns** — `"48 8B 45 ?? E8 ?? ?? ?? ??"` with `??` wildcards (distinct from literal
  `00`), converted to bytes+mask once.
- **Relative-instruction resolution** — `.RelativeOffset(n)` to follow a `lea`/`call` rip-relative
  displacement to its target (turns "the call inside curl_easy_setopt" into `Curl_vsetopt`'s address
  directly, no second scan).
- **String-ref discovery** — `FindStringRef(L"...")`: locate a `.rdata` string, then the `.text`
  `lea reg,[rip+disp]` that references it, then scan backward to the function prologue. This is how
  [Sinum](https://github.com/projectnovafn/Sinum/blob/main/Windows/Core/EOS.cpp) finds its HTTP hook
  (from the `"ProcessRequest failed. URL '%s' ..."` string) — far more update-stable than a raw byte
  prologue, and it mirrors the same strategy already added to `find_signature.py`.

Swap `CurlHook` / `GetBoneMatrix` resolution onto it, keeping `FindSignature` as the fallback.
**Files.** a new `utils/Memcury.h` (or a trimmed vendored copy), `network/CurlHook.h`, `ue/Engine.cpp`.
**Size.** Medium; pure infrastructure, testable offline against the same signatures.

### Hook `curl_setopt` (Curl_vsetopt) too — DONE

Platanium covers both the public `curl_easy_setopt` and the internal
`curl_setopt`/`Curl_vsetopt(CURL*, CURLoption, va_list)` it forwards to. `CurlHook` now does the same:
after finding `curl_easy_setopt`, it follows the single near-call in its prologue to resolve
`Curl_vsetopt` and hooks that too (`HookedVSetOpt`), applying the same URL rewrite + SSL-verify bypass
by patching the first va_list slot in place (x64: `*(void**)args`). This catches setopt calls made
through curl's internal path, not just the public wrapper. The two hooks coexist safely because both
rewrites are idempotent (a re-checked URL / already-0 verify is a no-op). **Files.**
`network/CurlHook.h`.

## SDK-surfaced ideas (from the PortalWars class survey)

New entries from a survey of `APortalWarsCharacter`, `UPortalWarsLocalPlayer`,
`APortalWarsPlayerState`, `APortalWarsPlayerController`, `FAutoAimData`, the skin/portal classes, and
`EReplayCameraMode`. Offsets/signatures are as of the current [Engine.h](../Internal/ue/Engine.h)
(cited by line). **Backend caveat:** the game is multiplayer, so `Server*` RPCs are validated
server-side — most of these only take effect against the [private emulator backend](backend-redirect.md),
not a real match; each entry says whether it's client-only (always works) or server-gated.

### ESP: player state read-through (K/D, rank, killstreak, bot flag) — PARTIAL (bot flag done)

**Bot flag shipped:** `ActorCache` caches `PlayerState->bIsABot`; ESP has a **Bot tag** and the
aimbot/triggerbot an **Ignore bots** filter. K/D / rank / killstreak read-through below is still open.


**Goal.** Richer ESP labels and target filtering from the player state, no extra calls.
**Approach.** Each character caches `LastPlayerState` (`APortalWarsCharacter` +0xdc0, Engine.h L1022)
— a plain pointer, no ProcessEvent. From it read `PlayerStats` (`FPlayerStatsInfo_InDepth` +0x4f8,
L1237: kills/deaths/assists/headshots), `KillStreak` (+0x358), and the base `APlayerState` `bIsABot`
bit (+0x22a) to tag bots. Add ESP text elements (K/D, streak, "BOT") and an aimbot/ESP "ignore bots"
/ "only bots" filter. Fold the reads into `ActorCache::Player` (one struct grow) so ESP/aim share
them. **Files.** `cache/ActorCache.h`, `features/Esp.h`, `features/Aimbot.h`, `settings/Settings.h`,
`menu/sections/Visuals.h`. **Client-only** (pure reads). **Depends on:** confirming the stat/bot
offsets in-game.

### Aim: weapon aim-assist / magnetism boost — Small–medium

**Goal.** A soft "legit" aim by amplifying the game's own aim-assist instead of moving the view.
**Approach.** Each `AGun` carries `FAutoAimData AutoAimConfig` at +0x460 (struct at Engine.h L1669:
`AutoAimRadius`, `MagnetismRange`, `MagnetismAngle`, `ShouldUseMagnetism`, `OnTargetTurnRate`, …).
For `CurrentWeapon` (character +0x800), each frame write enlarged radius/magnetism values (and force
`ShouldUseMagnetism = true`). Plain field writes, no ProcessEvent. Cache originals; restore on
`Destroy()`. **Files.** `ue/Engine.h` (`FAutoAimData`/`AGun` already present), a feature,
`settings/Settings.h`, `menu/sections/Aim.h`. **Client-only for the read; the assist itself runs in
the client aim path** (server sees only the resulting aim). **Depends on:** in-game tuning of the
inflated values.

### Camera: spectator-camera free-cam / third-person — Medium (was Large)

**Goal.** The free-cam / custom third-person the Cameras section wants, without RE'ing the native
camera-update function.
**Leads found.** `APortalWarsPlayerController::ClientSetSpectatorCamera(FVector, FRotator)` (L3425)
sets an arbitrary camera pose directly; the character already owns a built `ThirdPersonCamera` /
`ThirdPersonCameraArm` (+0x798 / +0x7a0, L880–881) and `SpectatorFirstPersonCamera` (+0x7b0); and
`OnReplayCameraModeChanged(EReplayCameraMode)` (L1052) drives a mode switch. **Approach.** Try
driving `ClientSetSpectatorCamera` each frame from a free-fly pose (WASD-integrated), or set the local
player's view target to the character's `ThirdPersonCamera`. Both avoid the blocked native camera
hook. **Files.** `ue/Engine.*` (wrap `ClientSetSpectatorCamera`), `features/FreeCam.h` /
`features/ThirdPerson.h`, `settings/Settings.h`. **Client-only** (view is local). **Depends on:**
in-game test that the pose sticks (the game may re-assert its camera each frame — may need a per-frame
re-apply or a view-target swap). `EReplayCameraMode` values still need an enum dump.

### Cosmetics: skin / loadout changer (refines the existing Large entry) — Medium–large

**Concrete API found.** Character holds `CharacterSkin` / `CharacterSkinClass` (+0x9b8 / +0x9c0,
L916) and `JetpackSkin` (+0x9d0); `UpdateSkins()` (L1029) re-applies them; gun holds
`WeaponSkin` (`ABaseGunSkin*` +0x2a8, L1656). `UPortalWarsLocalPlayer` has `UserSaveGameData`
(`UPortalWarsSaveGame*` +0x5b0, L1938) with `EquippedCustomizations` (+0x2a0) and
`LoadUserSaveGame()` (L1945) / `GetUserSaveGame()` (L1946). **Approach.** Resolve a skin class by name
(the SDK-tab `FindObject`), write it into `CharacterSkinClass` / `WeaponSkin`, call `UpdateSkins()`.
For persistent loadout, edit `EquippedCustomizations` then `LoadUserSaveGame()`. **Client-only visual**
if the mesh swap is local; **server-gated** if cosmetics are server-authoritative (test on the private
backend). Supersedes the approach notes in the Cosmetics changer entry below.

### Chat: sender + spammer, and an inbound-chat script event — Small–medium

**Goal.** Send chat programmatically (spam, callouts, command triggers) and expose incoming chat to
scripts.
**Approach.** Outgoing: the hand-added `APortalWarsPlayerController::SendChatMessage(FString, EChatType)`
helper (L3449, over `ServerBroadcastChatMessage`, L3413) is already used by the announce feature —
add a manual "send message" box + optional repeat/spam feature. Inbound: `ClientUpdateChat(FTextChatData)`
(L3424) flows through the existing `ProcessEvent` event bus — match it, parse `FTextChatData`
(sender/text at L3354) and dispatch a scripting `on_chat` event (ties into the scripting expansion) and
an optional in-menu chat log. **Files.** a feature / `menu/sections/Misc.h`,
`scripting/` (`on_chat` event), `hook/functions/ProcessEvent.h`. Outgoing chat is **server-gated**
(broadcast RPC); inbound read + script event is **client-only**.

### QoL: respawn / suicide button (+ script binding) — Small

**Goal.** The `respawn()` the scripting roadmap left open, plus a menu button.
**Approach.** `APortalWarsCharacter::RequestSuicide()` (L1048) / `ServerRequestSuicide()` (L1040), or
controller `CheatRespawn()` (L3444). Wrap one in `Engine.cpp`, add a Misc button and a
`player.respawn()` script binding. **Files.** `ue/Engine.*`, `menu/sections/Misc.h`,
`scripting/modules/Player.h`. **Server-gated** (respawn RPC; works on the private backend / when cheats
are enabled).

### Exploits: FOV slider + time-dilation (slomo) toggle — Small

**Goal.** Live FOV control and a slomo toggle.
**Approach.** FOV: `ServerSetFOV(float)` (L3400) or the save-game `FOV` (+0x34) already seeded at
init — a live slider that re-applies. Slomo: `ServerSlomo(float TimeDilation)` (L3398) /
`CheatSlowmo(float)` (L3442). Wrap in `Engine.cpp`, add Exploits/Misc controls. **Files.**
`ue/Engine.*`, `settings/Settings.h`, `menu/sections/Exploits.h`. **Server-gated** (both are `Server*`
/ cheat RPCs — private-backend only).

### Projection: `ProjectWorldLocationToScreenCustom` as a W2S fallback — Small

**Goal.** A PortalWars-specific projection to cross-check the native math W2S.
**Approach.** `APortalWarsPlayerController::ProjectWorldLocationToScreenCustom(FVector, FVector2D&, bool)`
(L3417) — wrap it as an alternative behind the existing `NativeWorldToScreen` Debug toggle chain, useful
when validating drift. **Files.** `ue/Engine.*`, `utils/WorldToScreen.h`. **Client-only.** Low priority
(the native math path already works).

### Note — server-gated RPCs and the private backend

Many surfaced actions are `Server*` RPCs (`ServerEquipWeapon`, `ServerTeleport`, `ServerSetFOV`,
`ServerKickPlayer`, `PortalLauncher::ServerAttemptToSpawnPortal`, …) or `Cheat*` execs gated behind
`ServerEnableCheats()` (L3412). Against a real match these are validated/rejected; against the
[self-hosted backend](backend-redirect.md) they're worth trying (and the character/controller `Cheat*`
execs — `CheatGodMode`, `CheatESP`, `CheatShootThroughWalls`, `CheatSlowmo` — may just work once cheats
are enabled). Treat each as "private-backend / RE experiment", not a blind-ship feature.

## Suggested sequencing

1. **Native WorldToScreen** — biggest standalone perf win, unblocks cheaper drawing everywhere.
2. **Recent logs panel** + **`SettingsChanged` payload** — small, high quality-of-life.
3. **Glow** — self-contained visual feature.
4. **Scripting API v2** (read actors → player control → UE utils → draw) — one themed pass; draw depends
   on the renderer (feature 6) and native WorldToScreen.
5. **Spawn/despawn-diff cache** — with the first static-object feature.
6. **Engine SDK refactor** — last; large and touchy, do it when features aren't in flight.
