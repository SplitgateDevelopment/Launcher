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

### Available events

| Event         | Fired from          | When                                  |
| ------------- | ------------------- | ------------------------------------- |
| `Render`      | UserScripts feature | Every rendered frame (while enabled). |
| `Shutdown`    | ProcessEvent        | Game instance is shutting down.       |
| `LoadIntoMap` | "Load into map" button | Button pressed.                    |

More game events (player death, spawn, kills, ...) are wired through a table in
[`hook/functions/ProcessEvent.h`](../Internal/hook/functions/ProcessEvent.h)
that maps an `Events::Type` value to a **UFunction full name**. To add one:

1. Add a value to `Events::Type` in
   [`scripting/Events.h`](../Internal/scripting/Events.h) (and to the pybind enum
   in `modules/Events.h`).
2. Enable `LogProcessEvent` (Debug section), trigger the action in-game, and note
   the `Function [...]` name printed for it.
3. Add a row to the `gameEvents` table, e.g.
   `{ Events::Type::PlayerDeath, "Function PortalWars.PortalWarsCharacter.OnDeath" }`.

The table is resolved to function pointers once and matched with a single map
lookup per call, skipped entirely when scripting is off or nothing is
subscribed, so it stays cheap on the very hot ProcessEvent path.

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
