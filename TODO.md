# TODO — changes needed in forbidden files

These edits are in files I can't touch (`Launcher/Launcher.cpp`). Everything else for each
item is already in place.

## Wire the crash handler into the launcher

`Launcher/utils/ExceptionHandler.h` is ready (mirrors the DLL's `Internal/utils/ExceptionHandler.h`:
symbolized stack-trace reports to `Documents\SplitgateInternal\Crashes`, progress logged through
the launcher's logger). It just needs installing from the entry point.

In `Launcher/Launcher.cpp`:

1. Add the include near the other `utils/` includes:

   ```cpp
   #include "utils/ExceptionHandler.h"
   ```

2. Right after the launcher's `Logger` instance is created (the one that attaches the console /
   `launcher.log`), install the handler, passing that logger:

   ```cpp
   Launcher::ExceptionHandler::Init(logger); // logger = the existing Logger instance
   ```

   `Init` captures the logger by reference, so the logger must outlive the process (a
   function-local in the entry point that lives for the whole run is fine). It defaults to
   `ExitMode::Silent` (write the report, then terminate without a WER dialog); pass
   `Launcher::ExceptionHandler::ExitMode::Crash` if you'd rather let the crash propagate.

3. Optionally, before a normal exit, `Launcher::ExceptionHandler::Disable();` (not required — the
   process is ending anyway).
