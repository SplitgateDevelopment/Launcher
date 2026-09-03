# Scripting

`Internal.dll` embeds **CPython** (via [pybind11](https://github.com/pybind/pybind11))
so users can extend it with Python scripts. The host code lives in
[`Internal/scripting/`](../Internal/scripting/).

## The `SplitgateInternal` module

An embedded module is exposed to every script:

```python
import SplitgateInternal

SplitgateInternal.Logger.Log("INFO", "hello from a script")
SplitgateInternal.Settings.Save()
```

It is assembled in [`Scripts.h`](../Internal/scripting/Scripts.h) from submodules
under [`scripting/modules/`](../Internal/scripting/modules/):

| Submodule  | Exposes                                  |
| ---------- | ---------------------------------------- |
| `Logger`   | `Log(level, message)`                    |
| `Settings` | `Save()`, `Reset()`                      |

Each submodule is a `void Scripts::Modules::X(py::module_& m)` that calls
`m.def_submodule(...)` and binds functions — the pattern to copy when adding a
new one.

## Where scripts live

```
%USERPROFILE%\Documents\SplitgateInternal\UserScripts\*.py
```

The folder is created on first run. Every `.py` file (except `__init__.py`) is
discovered and imported as a module `UserScripts.<name>`.

## Writing a script

A script must define a top-level **`main()`** function:

```python
# UserScripts/hello.py
import SplitgateInternal

def main():
    SplitgateInternal.Logger.Log("INFO", "tick")
```

## Execution model

`Scripts::Init()` discovers and imports each script once at startup
(`loadedScripts`). The **UserScripts** feature (see [features.md](features.md)),
while enabled, calls `Scripts::Execute(i)` for every loaded script every rendered
frame — i.e. each script's `main()` runs at the frame rate. Enable it via
`Settings.MISC.UserScriptsEnabled`.

`Scripts::ExecuteUnloaded(filename)` imports and runs a single script ad-hoc
without adding it to the loaded set.

## Events

Besides the per-frame `main()`, scripts can subscribe to **named events** and run
only when they fire, via the `Events` submodule:

```python
# UserScripts/on_shutdown.py
import SplitgateInternal

def handle():
    SplitgateInternal.Logger.Log("INFO", "bye")

SplitgateInternal.Events.on(SplitgateInternal.Events.Shutdown, handle)
```

Events are an enum (`Events::Type`), exposed to Python as `SplitgateInternal.Events.<Name>`
(`Render`, `Shutdown`, `LoadIntoMap`, ...). Register handlers at import time
(top-level code), not inside `main()`. A throwing handler is caught and logged,
so it can't crash the game.

A handler may take **no arguments**, or **one argument** to receive the event's `Payload`
(its arity is detected once at registration, so both styles keep working):

```python
def on_kill(payload):
    # payload.source / payload.target are raw game-object addresses (ints);
    # payload.value is event-specific (1.0 for a headshot on PlayerKilled).
    SplitgateInternal.Logger.Log("INFO", "headshot!" if payload.value else "kill")

SplitgateInternal.Events.on(SplitgateInternal.Events.PlayerKilled, on_kill)
```

### Available events

| Event             | Fired from             | When                                  |
| ----------------- | ---------------------- | ------------------------------------- |
| `Render`          | UserScripts feature    | Every rendered frame (while enabled). |
| `Shutdown`        | ProcessEvent           | Game instance is shutting down.       |
| `LoadIntoMap`     | "Load into map" button | Button pressed.                       |
| `SettingsChanged` | Menu                   | A setting changed (also on Reload/Reset). |
| `MenuOpened`      | Menu                   | GUI shown (Ins).                      |
| `MenuClosed`      | Menu                   | GUI hidden (Ins).                     |
| `PlayerDeath`     | ProcessEvent           | A character died (`payload.source` = the character). |
| `HealthChanged`   | ProcessEvent           | A character's health replicated.      |
| `DamageTaken`     | ProcessEvent           | Local player took damage.             |
| `RoundEnded`      | ProcessEvent           | A round ended.                        |
| `MatchEnded`      | ProcessEvent           | The match ended.                      |
| `PlayerKilled`    | ProcessEvent           | A kill happened — `payload` carries killer (`source`), victim (`target`), headshot (`value`). |

The game events are wired through a table in
[`hook/functions/ProcessEvent.h`](../Internal/hook/functions/ProcessEvent.h) that maps an
`Events::Type` value to a **UFunction full name** (the names come from the
[Dumpspace dump](game-dump.md)). For the generic ones the payload's `source` is the calling
UObject; a richer event like `PlayerKilled` has a dedicated block that decodes the call's
params into the payload. To add another:

1. Add a value to `Events::Type` in
   [`scripting/Events.h`](../Internal/scripting/Events.h) (and to the pybind enum
   in `modules/Events.h`).
2. Find the `Function [...]` name — search the dump, or enable `LogProcessEvent`
   (Debug section), trigger the action in-game, and read it off.
3. Add a row to the `gameEvents` table, e.g.
   `{ Events::Type::PlayerSpawn, "Function PortalWars.PortalWarsCharacter.OnSpawn" }`.

The table is resolved to function pointers once and matched with a single map lookup per call,
**skipped entirely when nothing is subscribed**, so it stays cheap on the very hot
ProcessEvent path. (Dispatch is no longer gated on `UserScriptsEnabled`, so C++ subscribers —
features, the shutdown teardown — receive game events too.)

### How it works

The registry lives in [`scripting/Events.h`](../Internal/scripting/Events.h)
(a Python-free `Events::Type -> handlers` map, unit tested in
`Tests/EventsTests.cpp`). The [`Events`
submodule](../Internal/scripting/modules/Events.h) bridges Python callables onto
it. Dispatch happens on the game/render thread inside the hooks, so handlers must
stay quick — the same constraint as features.

## Requirements

Building `Internal` requires a matching CPython install (see the main
[README](../README.md) for how the Python root is resolved). The `pythonXX.dll`
present at runtime must match the version compiled against.
