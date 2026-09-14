#!/usr/bin/env python3
"""
known_signatures.py - re-derive / verify every AOB signature the DLL relies on, in one pass.

Drives Tools/find_signature.py against the game binary for each signature the project hard-codes,
so after a game update you run one command, eyeball that each is still unique, and paste the emitted
C++ vector back into the source file noted per entry.

Signatures covered:
  - curl_easy_setopt       -> Internal/network/CurlHook.h   (Network::Curl::signature)
  - GetBoneMatrix          -> Internal/ue/Engine.cpp         (GetBoneMatrixSig)

Usage:
  python Tools/known_signatures.py                 # uses SPLITGATE_EXE or the default path
  python Tools/known_signatures.py --exe game.exe
"""

import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import find_signature as fs  # noqa: E402

DEFAULT_EXE = os.environ.get(
    "SPLITGATE_EXE",
    r"D:\Steam\steamapps\common\Splitgate\PortalWars\Binaries\Win64\PortalWars-Win64-Shipping.exe",
)

# name -> (source location, callable(pe))
SIGNATURES = {
    "curl_easy_setopt": (
        "Internal/network/CurlHook.h (Network::Curl::signature)",
        lambda pe: fs.strat_callsite(
            pe, "edx", fs.parse_ranges(["10000-10300", "20000-20120", "30000-30060"]),
            window=45, sig_len=36),
    ),
    "GetBoneMatrix": (
        "Internal/ue/Engine.cpp (GetBoneMatrixSig)",
        # The hand-wildcarded prologue from Engine.cpp, with its sub-rsp disp as ?? wildcards.
        lambda pe: fs.strat_bytes(
            pe,
            "48 8B C4 55 53 56 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC ?? ?? ?? ?? "
            "0F 29 78 B8 33 F6 44 0F 29 40",
            sig_len=34),
    ),
}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--exe", default=DEFAULT_EXE, help="game exe path (or set SPLITGATE_EXE)")
    args = ap.parse_args()

    if not os.path.exists(args.exe):
        sys.exit(f"exe not found: {args.exe}\nPass --exe or set the SPLITGATE_EXE env var.")

    pe = fs.PEImage(args.exe)
    print(f"loaded {args.exe}\n")

    for name, (where, finder) in SIGNATURES.items():
        print("=" * 78)
        print(f"# {name}  ->  {where}")
        print("=" * 78)
        try:
            finder(pe)
        except SystemExit as e:  # a strategy couldn't find it - report and keep going
            print(f"!! {name}: {e}")
        print()


if __name__ == "__main__":
    main()
