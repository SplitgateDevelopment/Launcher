# UE4 reverse-engineering cheatsheet

A personal, **living** reference for Unreal Engine 4 game internals — the offsets, patterns, and
snippets that come up again and again when writing an internal cheat. It's engine-general (not
Splitgate-specific); where a line already has a counterpart in this repo it's cross-linked so you
can see the working version.

> [!NOTE]
> Offsets are **build-specific**. Treat every literal here as "where to look," not a guaranteed
> value — confirm against the current dump ([game-dump.md](game-dump.md)) or a live RE pass. UE4 and
> UE5 differ in several places (called out below).

## Key objects

| What | Where | In this repo |
| --- | --- | --- |
| Camera POV (location/rotation/FOV) | `APlayerCameraManager->CameraCachePrivate` (UE 4.24+; the public `CameraCache` is a deprecated mirror that can be stale) | [WorldToScreen.h](../Internal/utils/WorldToScreen.h) picks the freshest of `CameraCachePrivate` / `CameraCache` by timestamp |
| Memory (silent) aimbot | write `APlayerController->ControlRotation` | [Aimbot.h](../Internal/features/Aimbot.h) eases `ControlRotation` toward the target |
| Entity list | `AGameState->PlayerArray` (players) or `ULevel->AActors` (all actors) | [ActorCache.h](../Internal/cache/ActorCache.h) walks `World->Levels[i]->Actors` |
| Actor id / unique id | `Actor + 0x18` (holds true for ~99% of UE4 games) | — |
| Actor mesh | `ACharacter->Mesh` → `USkeletalMeshComponent` | [Esp.h](../Internal/features/Esp.h) / [ActorCache.h](../Internal/cache/ActorCache.h) via `character->Mesh` |
| Relative location | `AActor->RootComponent->RelativeLocation` | ESP uses world location via `K2_GetActorLocation`; relative-location read is the ProcessEvent-free alternative |

## Offsets you derive from a neighbour

These are found relative to a field you already know, so they survive minor version bumps better than
absolute offsets.

- **Bone array** — relative to `USkeletalMeshComponent->LODData`:
  - **UE4:** `bone array = LODData - 0x4`
  - **UE5:** `bone array = LODData - 0x8` (and the `FMatrix` elements are **doubles**, not floats)
  - *Example (the value in the original note):* `LODData` at `0x4a8` → bone array at `0x4a0` (that
    example is the `-0x8` / UE5 layout).
  - In this repo bones are read the easy way instead — `GetBoneMatrix` is resolved by AOB and called
    natively ([Engine.cpp](../Internal/ue/Engine.cpp)), so the bone-array offset isn't needed here.
    Reading the array directly is the fully-offset (no function-call) alternative.
- **LastSubmitTime** — `UPrimitiveComponent->BoundsScale + 0x4`.
  - *Example:* `BoundsScale` at `0x284` → `LastSubmitTime` at `0x288`.
- **LastRenderTimeOnScreen** — `UPrimitiveComponent->BoundsScale + 0x6`.
  - *Example:* `BoundsScale` at `0x284` → `LastRenderTimeOnScreen` at `0x290`.
  - These two power a cheap, trace-free **visibility check** (see the snippet below and
    [roadmap.md](roadmap.md#aimbot-visibility-check-toggle)).

## Snippets

### Visibility check (no world trace)

Compares the mesh's last on-screen render time against its last submit time; if it rendered within a
tick, it's visible. Cheap (two field reads), engine-side, no `LineTraceSingle`.

```cpp
bool IsVisible(USkeletalMeshComponent* mesh)
{
    const float lastSubmit   = *(float*)((uintptr_t)mesh + lastSubmitOffset);
    const float lastRendered = *(float*)((uintptr_t)mesh + lastRenderOffset);
    constexpr float visionTick = 0.06f;
    return lastRendered + visionTick >= lastSubmit;
}
```

> Caveat: "rendered" means *drawn this frame anywhere on screen*, which is why the `visionTick`
> slack exists. It can read as visible through thin geometry or at frame edges — a line trace is
> stricter but costs a `ProcessEvent`. Good as the default, with the trace as an optional strict mode.

### Resolve an FName by id (GNames)

The GNames pool walk — chunk index in the high 16 bits, entry offset in the low 16.

```cpp
std::string GetNameFromId(int id, uintptr_t base)
{
    const DWORD chunk = ((unsigned)id >> 16);        // block
    const WORD  entryIdx = (WORD)id;
    const uintptr_t chunkPtr = *(uintptr_t*)(base + GNAMES_OFFSET + ((chunk + 2) * 8));
    const uintptr_t entry = chunkPtr + (DWORD)(2 * entryIdx);
    int len = *(INT16*)(entry) >> 6;                 // top 10 bits = length
    if (len > 256) len = 255;
    char buf[256] = {};
    RtlCopyMemory(buf, (void*)(entry + 2), len);
    buf[len] = 0;
    return buf;
}
```

This repo already resolves names through the SDK's `FName`/`FNamePool` path
([Engine.cpp](../Internal/ue/Engine.cpp), see [game-dump.md](game-dump.md) for the `GNames` offset) —
this raw version is the from-scratch equivalent for a game with no dumped SDK.

### Direction → UE rotation (aim math)

```cpp
Vector3 VectorToRotation(Vector3 v)
{
    constexpr float radToUnrRot = 57.2957795f; // 180/pi
    return Vector3(
        atan2(v.z, sqrt(v.x * v.x + v.y * v.y)) * radToUnrRot, // pitch
        atan2(v.y, v.x) * radToUnrRot,                          // yaw
        0.f);                                                   // roll
}
Vector3 GetAngleToActor(Vector3 target, Vector3 self) { return VectorToRotation(target - self); }
```

Identical math to [`Aimbot::DirToRotator`](../Internal/features/Aimbot.h) here (pitch = `atan2(z,
hypot(x,y))`, yaw = `atan2(y, x)`, in degrees).

## See also

- [game-dump.md](game-dump.md) — the concrete Splitgate offsets/SDK these patterns map onto.
- [roadmap.md](roadmap.md) — features these unlock (visibility check, glow, offset-only bone read).
- [hooking.md](hooking.md) — how functions (`ProcessEvent`, `PostRender`, and the AOB-scanned ones)
  are hooked.
