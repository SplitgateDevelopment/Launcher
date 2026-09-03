# TODO — changes needed in forbidden files

These edits are in files I can't touch (`Launcher/Launcher.cpp`). Everything else for each
item is already in place.

## Pass the game PID to the mitmproxy spawn

`Launcher::Mitmproxy::Spawn` now takes the game's process id as a second argument and uses it to
run a watchdog that exits mitmdump when the game closes (mitmdump also runs hidden now). The
launcher already has this PID — it's the one from `GetWindowThreadProcessId(gameWindow, &pid)`
used during injection.

In `Launcher/Launcher.cpp`, at the `Mitmproxy::Spawn` call site, pass that PID:

```cpp
// before
Launcher::Mitmproxy::Spawn(network.Redirects);
// after
Launcher::Mitmproxy::Spawn(network.Redirects, gamePid); // gamePid = the game's process id
```

If the PID isn't in scope there, use the process id already obtained from the game window (the
same `GetWindowThreadProcessId` out-param). Passing nothing still works (mitmdump stays hidden
but won't auto-close with the game).
