# TODO — changes needed in forbidden files

These edits are in files I can't touch (`Launcher/Launcher.cpp`). Everything else for each
item is already in place.

## Pass the game PID (and logger) to the mitmproxy spawn

`Launcher::Mitmproxy::Spawn` now takes the game's process id and an optional logger:

```cpp
bool Spawn(const std::map<std::string, std::string>& redirects, DWORD gamePid = 0, Shared::Logger* logger = nullptr);
```

- `gamePid` — the game's process id (the one from `GetWindowThreadProcessId(gameWindow, &pid)`
  used during injection). Lets the watchdog addon exit mitmdump when the game closes.
- `logger` — the launcher's logger, so Spawn can report what happened (started / mitmdump not on
  PATH / **addon scripts not found**).

At the `Mitmproxy::Spawn` call site in `Launcher/Launcher.cpp`:

```cpp
// before
Launcher::Mitmproxy::Spawn(network.Redirects);
// after
Launcher::Mitmproxy::Spawn(network.Redirects, gamePid, &logger); // gamePid = game process id, logger = the launcher Logger
```

Treat a `false` return as **non-fatal** in Mitmproxy mode — Spawn already logs the specific
reason (e.g. the bundled `scripts/` folder was deleted), so just log/continue rather than
aborting the launcher. Passing nothing still compiles (mitmdump stays hidden, no watchdog, no
logging).
