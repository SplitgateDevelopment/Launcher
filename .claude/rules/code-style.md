# Code Style

C++ conventions for this repository. Apply them when adding or editing code.

## Naming

- **PascalCase** for file names, namespaces, and class/struct names.
  - Evidenced by the project's headers/sources: `DrawActors.h`, `PlayerModifications.h`,
    `ProcessEvent.h`, `Engine.cpp`, `Settings.cpp`.
- **camelCase** for variables — locals, parameters, and members.
- **lowercase** for directory names (`hook/`, `menu/gui/`, `scripting/modules/`,
  `settings/`, `utils/`).

## Control flow

- **Always early-return.** Guard against invalid state at the top of a function and bail
  out (`return` / `continue` / `break`) instead of nesting the happy path inside `if`
  blocks. Prefer a flat sequence of guard clauses over an `else` ladder, so the main logic
  stays at the lowest indentation level.
  - Evidenced by `Logger::_log` (returns as soon as the log file isn't open) and the
    `Hook::Init` bootstrap (each step bails on failure before the next).

## Documentation comments

Document the public surface with **Doxygen comments** — the C++ equivalent of TSDoc/JSDoc.
Apply them when adding or editing code.

- **Every file** opens with a brief saying what it contains and why it exists.
- **Namespaces, classes/structs, and functions** get a doc comment. Non-obvious struct/class
  members get a trailing `///< ...`.
- Use `///` for one-line briefs; use a `/** ... */` block with `@param` / `@return` when a
  function needs them. Document the **contract** — ownership, side effects, units, threading,
  what may be null — not the obvious (skip `@return the value` on a trivial getter).
- Don't delete existing explanatory comments; fold them into the doc comment.
- **Generated / third-party code** (the UE SDK in `ue/`, the discord-rpc wrappers) gets a
  **file-level** brief only, not per-symbol docs.

```cpp
/**
 * Removes the settings file so the next launch starts from defaults.
 *
 * @param backup  when true, the file is renamed to *.bak instead of deleted.
 * @return true if a file was removed (or renamed), false if none existed.
 */
bool Delete(bool backup = false);

/// Screen-space bounding box for one actor, in pixels.
struct Box
{
    float x;       ///< left edge
    float width;   ///< in pixels, already DPI-scaled
};
```

## Namespaces

- Group related functionality into **reusable namespaces** rather than free-floating
  globals or one-off helpers. Prefer a namespace that can be included and reused across
  features over duplicating logic in each call site.

## Exceptions

- Files that wrap third-party code keep the upstream naming instead of PascalCase —
  e.g. `discord/handlers.h` and `discord/rpc.h` follow the discord-rpc convention.
