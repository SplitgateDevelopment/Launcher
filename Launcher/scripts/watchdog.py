"""
Watchdog addon: exits mitmdump when the game process ends, so the hidden proxy
doesn't linger after the game closes.

The launcher passes the game's PID via the SPLITGATE_GAME_PID environment variable.
Always appended alongside the main addon, whatever mode it is.
"""

import ctypes
import os
import threading
import time


def _watch(pid):
    kernel32 = ctypes.windll.kernel32
    # Open the game process; retry briefly so a transient miss right at startup doesn't make us
    # tear mitmdump down prematurely.
    handle = None
    for _ in range(30):
        handle = kernel32.OpenProcess(0x00100000, False, pid)  # SYNCHRONIZE
        if handle:
            break
        time.sleep(1)
    if handle:
        kernel32.WaitForSingleObject(handle, 0xFFFFFFFF)  # block until the game exits
        kernel32.CloseHandle(handle)
    os._exit(0)


_pid = int(os.environ.get("SPLITGATE_GAME_PID", "0"))
if _pid:
    threading.Thread(target=_watch, args=(_pid,), daemon=True).start()
