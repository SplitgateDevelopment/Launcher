# Manual changes needed (forbidden files)

These edits are in files I'm not allowed to modify. Apply them when you're back.

## `Launcher/Launcher.cpp` — spawn mitmproxy for `ProxyMode::Mitmproxy`

The proxy helpers exist (`Launcher/utils/ProxyConfig.h`, `Launcher/utils/Mitmproxy.h`); they
just need wiring into `main()`.

1. Add the includes near the top:

   ```cpp
   #include "utils/ProxyConfig.h"
   #include "utils/Mitmproxy.h"
   ```

2. In `main()`, **before** launching/injecting the game, read the DLL's NETWORK settings and,
   in Mitmproxy mode, start mitmproxy:

   ```cpp
   const auto network = Launcher::ReadNetworkSettings();
   if (network.Proxy == ProxyMode::Mitmproxy)
   {
       if (Launcher::Mitmproxy::Spawn(network.Redirects))
           logger.success("Spawned mitmproxy");
       else
           logger.error("Failed to spawn mitmproxy (is mitmdump on PATH?)");
   }
   ```

Notes:
- `ProxyMode::Internal` and `ProxyMode::Manual` need **no** launcher action — the injected DLL
  handles Internal in-process; Manual does nothing (matches the old behavior).
- mitmproxy still needs the game to route through it (Windows system proxy → `127.0.0.1:8080`,
  or a transparent/WinDivert mode). The helper only launches `mitmdump`; see
  `docs/backend-redirect.md`.
- The launcher now uses vcpkg (nlohmann-json) — building it in VS runs `vcpkg install`
  automatically (a `Launcher/vcpkg.json` manifest was added).
