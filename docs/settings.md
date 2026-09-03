# Settings

Persistent configuration for `Internal.dll`, defined in
[`Internal/settings/Settings.h`](../Internal/settings/Settings.h) and
[`Internal/settings/Settings.cpp`](../Internal/settings/Settings.cpp).

## Layout

A single global `Settings` object (`extern SETTINGS Settings;`) groups the sections:

| Section    | Purpose                                   |
| ---------- | ----------------------------------------- |
| `MENU`     | GUI visibility, watermark, toggle hotkey  |
| `EXPLOITS` | Gameplay feature toggles and values       |
| `MISC`     | Player name, Discord RPC, user scripts    |
| `DEBUG`    | Logging and debug windows                 |
| `VISUALS`  | ESP elements + colors, radar toggle       |
| `NETWORK`  | Backend proxy mode, redirect map, HTTP logging (also read by the launcher) |

Each section is a plain struct with default-initialized members, e.g.:

```cpp
struct ExploitsSettings {
    float FOV = 80.f;
    bool  GodMode = false;
    ...
};
```

## Persistence

Serialization is handled by [nlohmann/json](https://github.com/nlohmann/json)
through the `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT` macro on each
struct. The `_WITH_DEFAULT` variant is important: a settings file that is
missing a key (an older file, or a partial one) still loads, with the absent
field falling back to its default instead of failing the whole load.

**Not every declared field is persisted.** Only the fields listed in a struct's
`NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(...)` are written and read.
Fields left out of that list (e.g. `MiscSettings::DiscordAppID`/`SteamAppId`) are
intentionally runtime/constant values that cannot be changed through the file.

### File location

Settings live at:

```
%USERPROFILE%\Documents\SplitgateInternal\splitgate.settings
```

resolved via `SHGetKnownFolderPath(FOLDERID_Documents, ...)`. The directory is
created on first save.

## API — `namespace SettingsHelper`

| Function                 | Description                                                        |
| ------------------------ | ----------------------------------------------------------------- |
| `bool Load()`            | Load the file into `Settings`. Returns `false` on missing/corrupt files (never throws). |
| `void Save()`            | Write `Settings` to disk as pretty-printed JSON.                  |
| `void Reset()`           | Reset `Settings` to defaults (`Settings = SETTINGS{}`).           |
| `void Delete()`          | Remove the settings file.                                         |
| `fs::path GetAppPath(std::string filename = "")` | Path inside the `SplitgateInternal` app folder. |
| `std::string GetSettingsFilePath()` | Full path to the settings file.                       |

`SettingsHelper` is a thin facade: the actual load/save is done by a generic
**`Shared::SettingsFile<T>`** ([`shared/Settings.h`](../shared/Settings.h)) bound to the global
`Settings`, and `Shared::AppDataPath` resolves the folder. The template opens/closes the file
per call (no lingering handle), so the same file can be read by another process — the
**launcher** reuses `SettingsFile<T>` with a narrow view to read the `NETWORK` section (proxy
mode / redirects) without the game-only sections. See
[`Launcher/utils/ProxyConfig.h`](../Launcher/utils/ProxyConfig.h) and
[backend-redirect.md](backend-redirect.md).

## Exposed to user scripts

The `Settings` submodule of the embedded `SplitgateInternal` Python module
exposes `Reset()` and `Save()` to user scripts — see
[scripting.md](scripting.md).

## Tests

Save/load, defaults, missing-key tolerance and the persisted-vs-runtime field
split are covered by `Tests/SettingsTests.cpp` — see [testing.md](testing.md).
