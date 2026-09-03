# Code Style

C++ conventions for this repository. Apply them when adding or editing code.

## Naming

- **PascalCase** for file names, namespaces, and class/struct names.
  - Evidenced by the project's headers/sources: `DrawActors.h`, `PlayerModifications.h`,
    `ProcessEvent.h`, `Engine.cpp`, `Settings.cpp`.
- **camelCase** for variables — locals, parameters, and members.
- **lowercase** for directory names (`hook/`, `menu/gui/`, `scripting/modules/`,
  `settings/`, `utils/`).

## Namespaces

- Group related functionality into **reusable namespaces** rather than free-floating
  globals or one-off helpers. Prefer a namespace that can be included and reused across
  features over duplicating logic in each call site.

## Exceptions

- Files that wrap third-party code keep the upstream naming instead of PascalCase —
  e.g. `discord/handlers.h` and `discord/rpc.h` follow the discord-rpc convention.
