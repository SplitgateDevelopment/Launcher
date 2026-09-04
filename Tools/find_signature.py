#!/usr/bin/env python3
"""
find_signature.py - locate a function in a PE and emit an AOB signature for it.

A small, reusable reverse-engineering helper for this project's AOB hooks (see
Internal/utils/Util.cpp, whose FindSignature treats 0x00 as a wildcard, and
docs/ue4-cheatsheet.md). It was distilled from the one-off script used to find
curl_easy_setopt in the Splitgate build (docs/backend-redirect.md).

Strategies
----------
- callsite : find the function that is the dominant target of `mov <reg>, <imm>; call`
             sites whose immediate matches a predicate. Great when a function is
             always called with a recognizable tagged constant (e.g. curl options).
- bytes    : find occurrences of a raw byte pattern ("48 89 .. ?? E8" - ".." / "??"
             are wildcards) and, for a unique match, emit a signature at that address.
- emit     : emit a masked signature for a function at a known RVA/VA.

Every strategy that emits a signature masks rel32 operands (E8/E9 calls/jmps) and
rip-relative disp32 with 0x00 (the project's wildcard) and verifies the result is
unique in .text before printing it as a C++ std::vector<BYTE> literal.

Requires: pip install capstone pefile

Examples
--------
  # curl_easy_setopt in the Splitgate build (the preset that found it):
  python Tools/find_signature.py callsite --exe game.exe \
      --reg edx --imm 10000-10300 --imm 20000-20120 --imm 30000-30060

  # a known prologue with wildcards:
  python Tools/find_signature.py bytes --exe game.exe --pattern "48 8B C4 48 89 58 ?? E8 ?? ?? ?? ??"

  # emit a signature for a function you already located:
  python Tools/find_signature.py emit --exe game.exe --rva 0x3e75a60 --len 40
"""

import argparse
import sys
from collections import Counter

try:
    import pefile
    from capstone import Cs, CS_ARCH_X86, CS_MODE_64
except ImportError:
    sys.exit("Missing dependency. Run: pip install capstone pefile")


class PEImage:
    """A loaded PE with convenient access to its .text section and address math."""

    def __init__(self, path: str):
        self.pe = pefile.PE(path, fast_load=True)
        self.image_base = self.pe.OPTIONAL_HEADER.ImageBase
        section = next((s for s in self.pe.sections if s.Name.rstrip(b"\x00") == b".text"), None)
        if section is None:
            sys.exit("No .text section found")
        self.text = section.get_data()
        self.text_rva = section.VirtualAddress
        self.md = Cs(CS_ARCH_X86, CS_MODE_64)

    @property
    def text_va(self) -> int:
        """Absolute VA of the start of .text."""
        return self.image_base + self.text_rva

    def off_to_va(self, off: int) -> int:
        return self.image_base + self.text_rva + off

    def va_to_off(self, va: int) -> int:
        return va - self.image_base - self.text_rva

    def disasm(self, off: int, count: int, size: int = 256):
        """Disassemble up to `count` instructions starting at file offset `off`."""
        out = []
        for n, ins in enumerate(self.md.disasm(self.text[off:off + size], self.off_to_va(off))):
            if n >= count:
                break
            out.append(ins)
        return out


# --------------------------------------------------------------------------- #
# Signature emission
# --------------------------------------------------------------------------- #

def mask_signature(pe: PEImage, entry_off: int, target_len: int):
    """
    Build a signature starting at entry_off, spanning whole instructions until at
    least target_len bytes, masking rel32 (E8/E9) and rip-relative disp32 with 0x00.
    Returns (raw_bytes, masked_bytes).
    """
    raw = bytearray()
    masked = bytearray()
    total = 0
    for ins in pe.md.disasm(bytes(pe.text[entry_off:entry_off + target_len + 16]), pe.off_to_va(entry_off)):
        b = bytearray(ins.bytes)
        m = bytearray(ins.bytes)
        if b and b[0] in (0xE8, 0xE9) and len(b) == 5:  # near call / jmp rel32
            m[1:5] = b"\x00\x00\x00\x00"
        if "[rip" in ins.op_str and len(b) >= 5:  # rip-relative disp32 = last 4 bytes
            m[-4:] = b"\x00\x00\x00\x00"
        raw += b
        masked += m
        total += len(ins.bytes)
        if total >= target_len:
            break
    return raw, masked


def count_masked(pe: PEImage, pattern: bytearray) -> int:
    """Count occurrences of a masked pattern (0x00 = wildcard) across .text."""
    data = pe.text
    fixed = [(i, pattern[i]) for i in range(len(pattern)) if pattern[i] != 0]
    n, m, cnt, start = len(data), len(pattern), 0, 0
    while start <= n - m:
        if all(data[start + i] == v for i, v in fixed):
            cnt += 1
        start += 1
    return cnt


def print_signature(pe: PEImage, entry_off: int, length: int):
    raw, masked = mask_signature(pe, entry_off, length)
    occ = count_masked(pe, masked)
    va = pe.off_to_va(entry_off)
    print(f"function  : rva=0x{va - pe.image_base:x} va=0x{va:x}")
    print(f"raw       : {' '.join(f'{x:02X}' for x in raw)}")
    print(f"masked    : {' '.join(f'{x:02X}' for x in masked)}")
    print(f"unique    : {occ} occurrence(s) in .text ({'OK' if occ == 1 else 'NOT UNIQUE - widen --len'})")
    print("C++       : inline const std::vector<BYTE> signature = {" +
          ", ".join(f"0x{x:02X}" for x in masked) + "};")
    return occ == 1


# --------------------------------------------------------------------------- #
# Strategies
# --------------------------------------------------------------------------- #

def parse_ranges(specs):
    """['10000-10300', '41'] -> predicate(imm) -> bool."""
    ranges = []
    for s in specs:
        if "-" in s:
            lo, hi = s.split("-", 1)
            ranges.append((int(lo, 0), int(hi, 0)))
        else:
            v = int(s, 0)
            ranges.append((v, v))
    return lambda imm: any(lo <= imm <= hi for lo, hi in ranges)


# `mov <reg>, imm32` opcodes: B8+rd. edx=BA, ecx=B9, eax=B8, r8d=41 B8, r9d=41 B9.
_MOV_IMM32 = {"eax": b"\xB8", "ecx": b"\xB9", "edx": b"\xBA", "ebx": b"\xBB",
              "r8d": b"\x41\xB8", "r9d": b"\x41\xB9"}


def strat_callsite(pe: PEImage, reg: str, imm_pred, window: int, sig_len: int):
    """Find the dominant `call` target preceded by `mov <reg>, <imm matching pred>`."""
    opc = _MOV_IMM32.get(reg)
    if not opc:
        sys.exit(f"Unsupported --reg {reg}; choose one of {', '.join(_MOV_IMM32)}")
    data = pe.text
    targets = Counter()
    routed_imms = {}
    k = 0
    while True:
        k = data.find(opc, k)
        if k < 0:
            break
        imm_off = k + len(opc)
        if imm_off + 4 > len(data):
            break
        imm = int.from_bytes(data[imm_off:imm_off + 4], "little")
        if imm_pred(imm):
            w = data[k:k + window]
            call = w.find(b"\xE8", len(opc) + 4)
            if call >= 0 and k + call + 5 <= len(data):
                rel = int.from_bytes(data[k + call + 1:k + call + 5], "little", signed=True)
                dst = pe.off_to_va(k + call) + 5 + rel
                targets[dst] += 1
                routed_imms.setdefault(dst, set()).add(imm)
        k += 1

    if not targets:
        sys.exit("No matching call sites found - widen --imm / --reg / --window.")

    print("top call targets:")
    for dst, cnt in targets.most_common(6):
        print(f"  0x{dst:x}  hits={cnt}")
    best = targets.most_common(1)[0][0]
    print(f"\nbest = 0x{best:x}; routed immediates: {sorted(routed_imms[best])[:24]}")
    print()
    print_signature(pe, pe.va_to_off(best), sig_len)


def strat_bytes(pe: PEImage, pattern: str, sig_len: int):
    """Find a raw pattern ('.'/'??' = wildcard); emit a signature at a unique match."""
    tokens = pattern.replace(",", " ").split()
    pat = bytearray()
    for t in tokens:
        pat.append(0 if t in ("??", "..", "?", ".") else int(t, 16))
    hits = []
    data = pe.text
    fixed = [(i, pat[i]) for i in range(len(pat)) if pat[i] != 0]
    for start in range(0, len(data) - len(pat)):
        if all(data[start + i] == v for i, v in fixed):
            hits.append(start)
            if len(hits) > 8:
                break
    print(f"pattern matches: {len(hits)}{' (showing first 8)' if len(hits) > 8 else ''}")
    for h in hits[:8]:
        print(f"  rva=0x{pe.text_rva + h:x} va=0x{pe.off_to_va(h):x}")
    if len(hits) == 1:
        print()
        print_signature(pe, hits[0], sig_len)


def strat_emit(pe: PEImage, rva: int, va: int, sig_len: int):
    if va:
        off = pe.va_to_off(va)
    elif rva is not None:
        off = rva - pe.text_rva
    else:
        sys.exit("emit needs --rva or --va")
    print_signature(pe, off, sig_len)


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    c = sub.add_parser("callsite", help="find a function by its tagged call sites")
    c.add_argument("--exe", required=True)
    c.add_argument("--reg", default="edx", help="register the tag is moved into (default edx)")
    c.add_argument("--imm", action="append", default=[], required=True,
                   help="immediate value or lo-hi range (repeatable), e.g. 10000-10300 or 0x2712")
    c.add_argument("--window", type=int, default=45, help="bytes to look ahead for the call (default 45)")
    c.add_argument("--len", type=int, default=40, dest="siglen")

    b = sub.add_parser("bytes", help="find a raw pattern and emit a signature at a unique match")
    b.add_argument("--exe", required=True)
    b.add_argument("--pattern", required=True, help="e.g. \"48 8B C4 ?? E8 ?? ?? ?? ??\"")
    b.add_argument("--len", type=int, default=40, dest="siglen")

    e = sub.add_parser("emit", help="emit a signature for a function at a known RVA/VA")
    e.add_argument("--exe", required=True)
    e.add_argument("--rva", type=lambda x: int(x, 0), default=None)
    e.add_argument("--va", type=lambda x: int(x, 0), default=None)
    e.add_argument("--len", type=int, default=40, dest="siglen")

    args = ap.parse_args()
    pe = PEImage(args.exe)
    print(f"loaded {args.exe}: image_base=0x{pe.image_base:x} .text va=0x{pe.text_va:x} "
          f"size=0x{len(pe.text):x}\n")

    if args.cmd == "callsite":
        strat_callsite(pe, args.reg, parse_ranges(args.imm), args.window, args.siglen)
    elif args.cmd == "bytes":
        strat_bytes(pe, args.pattern, args.siglen)
    elif args.cmd == "emit":
        strat_emit(pe, args.rva, args.va, args.siglen)


if __name__ == "__main__":
    main()
