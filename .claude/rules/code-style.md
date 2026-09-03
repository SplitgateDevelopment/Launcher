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

## Namespaces

- Group related functionality into **reusable namespaces** rather than free-floating
  globals or one-off helpers. Prefer a namespace that can be included and reused across
  features over duplicating logic in each call site.

## Exceptions

- Files that wrap third-party code keep the upstream naming instead of PascalCase —
  e.g. `discord/handlers.h` and `discord/rpc.h` follow the discord-rpc convention.
