# Features

A **feature** is a self-contained gameplay tweak (GodMode, InfiniteJetpack,
SpinBot, ...) driven once per rendered frame from the DirectX 11 present hook
(`PostRender`). The framework lives in
[`Internal/features/`](../Internal/features/).

## Anatomy

| File               | Role                                                        |
| ------------------ | ----------------------------------------------------------- |
| `Feature.h`        | Abstract base class every feature derives from.             |
| `FeatureRunner.h`  | The registry (`Features::Features`) and the per-frame loop (`Features::Execute`). Depends only on `Feature.h`, so it is unit-testable. |
| `Features.h`       | Includes every concrete feature and registers them in `Features::Init()`. |
| `*.h` (the rest)   | The concrete features.                                      |

## The `Feature` contract

```cpp
class Feature {
public:
    bool Enabled = false;      // toggled from settings via UpdateEnabled()
    bool Initialized = false;
    bool OneTime = false;      // Run() fires once per enable instead of every frame
    std::string Name = "BaseFeature";
    // applied / hasRun are framework bookkeeping — features must not touch them.

    virtual void Init() = 0;
    virtual void UpdateEnabled() = 0;
    virtual bool Check() = 0;
    virtual void Destroy() = 0;
    virtual void Run() = 0;
    void Log(std::string message);   // gated by Settings.DEBUG.FeaturesLogging
};
```

Each frame, `Features::Execute()` runs, for every registered feature:

```text
if (!Initialized) Init();
UpdateEnabled();               // refresh Enabled from settings
if (!Check()) continue;        // Check() == "is it valid/safe to act right now?"
if (Enabled)  Run();           // once, if OneTime
else if (applied) Destroy();   // revert, once, on the enabled -> disabled edge
```

Implement the virtuals as:

- **`Init()`** — one-time setup (cache pointers/originals); set `Initialized`.
- **`UpdateEnabled()`** — `Enabled = Settings.<section>.<flag>;`.
- **`Check()`** — return whether it is currently valid to act (pointers ready,
  in a match, ...). **Do not gate this on `Enabled` and do not return `Enabled`** —
  the loop decides Run vs Destroy.
- **`Run()`** — apply the effect. Called every frame while enabled, or once per
  enable if `OneTime`.
- **`Destroy()`** — revert whatever `Run()` applied. Called **once**, on the
  enabled → disabled edge.

### Why Check() must not return Enabled

Historically every feature's `Check()` ended with `return Enabled`. That made a
disabled feature return `false`, so the loop skipped it and **`Destroy()` never
ran** — disabling a feature never reverted its effect. Returning validity
instead fixes that and lets the loop revert exactly once on disable.

Features that still `return Enabled` keep working (they simply never reach
`Destroy()`), so migration can be incremental.

### One-time features

Set `OneTime = true` for a feature whose `Run()` should fire once per enable
rather than every frame. It re-arms when disabled.

### Event-driven features

Every feature has an `Event` (an `Events::Type`, default `Render`). `Render`
features run from the per-frame `Features::Execute` loop. A feature with any
other `Event` is instead subscribed to the [event bus](scripting.md#events) in
`Features::Init` and driven by `Features::RunFeature` when that event is
dispatched — e.g. set `Event = Events::Type::PlayerDeath` to run a feature when
the player dies. Same toggle/Check/Run/Destroy contract, just a different clock.

One-shot **actions** that don't need a toggle (like the "Load into map" button)
are better as a plain event handler than a feature — see the `LoadIntoMap`
handler registered in `Features::Init`, dispatched by the button via
`Events::Dispatch(Events::Type::LoadIntoMap)`.

## Adding a feature

1. Create `Internal/features/MyFeature.h` deriving from `Feature`.
2. `#include "MyFeature.h"` in `Features.h` and add
   `Features.push_back(std::make_unique<MyFeature>());` to `Features::Init()`.
3. Add the header to `Internal.vcxproj` / `.filters` (or let Visual Studio do it).

## Current features

GodMode, InfiniteJetpack, NoRecoil, SpinBot, PlayerModifications,
WeaponModifications, DrawActors (ESP), UserScripts (runs Python user scripts —
see [scripting.md](scripting.md)).

## Tests

The framework (init/check/run/destroy transitions, OneTime, legacy contract) is
covered by `Tests/FeaturesTests.cpp` using fake features — see
[testing.md](testing.md). The concrete features touch the live game and are not
unit tested.
