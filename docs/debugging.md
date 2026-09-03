# Debugging

Where the logs and crash reports live, how to read them, and the switches that make
debugging easier. Logging itself is implemented in [`shared/Logger.h`](../shared/Logger.h)
(see the launcher/DLL split in [hooking.md](hooking.md)); crash reporting is in
[`shared/ExceptionHandler.h`](../shared/ExceptionHandler.h).

## Where things are

Both components log to a **console** and to a **file**:

| Component | Console | Log file |
| --------- | ------- | -------- |
| **Launcher** (`Launcher.exe`) | its own console window | `launcher.log`, next to `Launcher.exe` (truncated each run) |
| **DLL** (`Internal.dll`) | a console it spawns on inject | `internal.log`, in `Documents\SplitgateInternal\` (truncated each run) |

The DLL's app folder — `Documents\SplitgateInternal\` — also holds:

- `splitgate.settings` — the saved settings JSON ([settings.md](settings.md)).
- `Crashes\<timestamp>\StackTrace.log` — one folder per crash (below).
- `Dumps\GObjects.txt` — the object dump from **Debug → Dump GObjects**.

## Reading a log line

Every line is `[HH:MM:SS] [LEVEL] message` (local time), colored by level in the console:

```
[17:23:48] [INFO] Loading...
[17:23:48] [SUCCESS] Injected
[17:23:48] [ERROR] No World
```

- `INFO` (blue), `SUCCESS` (green), `ERROR` (red), `RPC` (yellow, Discord). The color is
  console-only; the file gets the same text without color.
- The DLL's startup sequence (`Init` in [hooking.md](hooking.md) §2) logs each step, so the
  **last line before it stops** tells you which step failed (e.g. `No World`,
  `No LocalPlayer`, `MinHook not initialized`).

### The in-game console

The DLL's console can be hidden/shown with **Misc → Show console** (persisted as
`MISC.ShowConsole`). If you don't see it, that toggle is off. Closing the console window does
not unload the DLL.

## Reading a crash stack trace

On an unhandled exception the DLL writes `Crashes\<timestamp>\StackTrace.log` via the shared
crash handler, then (by default) deletes the settings file so the next launch starts clean.

A report looks like:

```
Exception: (0xc0000005)
== Stack Trace ==
  Internal.dll!Splitgate::Features::Execute    [C:\...\features\FeatureRunner.h:41]
  Internal.dll!PostRender::HookedPostRender    [C:\...\hook\functions\PostRender.h:20]
  Splitgate-Win64-Shipping.exe + 0x1a2b3c4
  ...
```

How to read it:

- **`0xc0000005`** is the exception code — `0xC0000005` is an access violation (the most
  common: a bad/null pointer, usually a game object that wasn't validated). Other codes:
  `0xC00000FD` stack overflow, `0xC000001D` illegal instruction.
- **Frames are top-to-bottom = innermost-first.** The **top frame is where it crashed**; the
  ones below are the callers. Start at the top.
- `module!symbol    [file:line]` — symbol + source location resolved from the module's **PDB**.
  Frames from `Internal.dll` will have `file:line` **only if `Internal.pdb` sits next to the
  DLL** (a Release build still produces one — keep it around). Frames in the game
  (`...Shipping.exe`) have no symbols, so they show as `module + 0xoffset`.
- Frames that can't be attributed to a module show only a raw `0xADDRESS`.

If your `Internal.dll` frames show `+ 0xoffset` instead of `symbol [file:line]`, the PDB is
missing or mismatched — rebuild and keep the matching `Internal.pdb` beside the DLL.

### Controlling the crash-recovery wipe

Deleting settings on crash is on by default (it recovers from a bad setting that crashes on
load), but it also throws away your config. Toggle it with **Debug → Delete settings on
crash** (`DEBUG.DeleteSettingsOnCrash`, default on). Turn it **off** when you're debugging a
crash and want to keep the settings that triggered it. The `StackTrace.log` is written either
way.

## Switches that help

Under the **Debug** tab (and persisted in the settings JSON):

| Toggle | Setting | Effect |
| ------ | ------- | ------ |
| Log ProcessEvent | `DEBUG.LogProcessEvent` | logs every hooked `ProcessEvent` (UFunction) call — very noisy, but shows the exact event/function names flowing through the game (useful for wiring new events, see [scripting.md](scripting.md)). |
| Features Logging | `DEBUG.FeaturesLogging` | logs feature init/run activity from the feature runner ([features.md](features.md)). |
| Show demo window / style editor | `DEBUG.ShowDemoWindow` / `ShowStyleEditor` | the ImGui demo / style editor windows. |
| Delete settings on crash | `DEBUG.DeleteSettingsOnCrash` | see above. |

**Dump GObjects** (Debug tab) writes every UObject's index and full name to
`Dumps\GObjects.txt` — handy for finding class/function names when reverse-engineering the
game (e.g. to add a new hooked event).

## Reproducing in a debugger

`Internal.dll` is injected, so to debug it live, attach your debugger to the **game process**
(not the launcher) after injection, and make sure `Internal.pdb` is loaded. The launcher is a
normal console app you can run/debug directly. Anything that only reproduces in-game can't be
covered by the [test suite](testing.md), which runs off the game.
