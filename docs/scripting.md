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

> **Note:** running every script every frame is the current model. An
> event-driven model (scripts subscribing to callbacks such as "on player death"
> instead of running each frame) is planned; this document will be updated when
> it lands.

## Requirements

Building `Internal` requires a matching CPython install (see the main
[README](../README.md) for how the Python root is resolved). The `pythonXX.dll`
present at runtime must match the version compiled against.
