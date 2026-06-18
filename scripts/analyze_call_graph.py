#!/usr/bin/env python3
"""
Analyze the ARM call graph in libg.so to determine which game logic functions
can be safely replaced vs which need trampolines.

For each game logic function, we determine:
  - CALLED BY: what other functions call it (internal ARM, game logic C++)
  - CALLS: what functions it calls (internal ARM, game logic C++)
  - Classification:
      SAFE: Leaf function — doesn't require a trampoline to replace
      NEEDS_TRAMPOLINE: Called by internal ARM code, so need a branch veneer
      DONT_TOUCH: Called internally in ways that break if you replace it

Usage:
  python3 analyze_call_graph.py original/libg.so symbols/game_logic_symbols.txt
"""

import subprocess
import re
import sys
from pathlib import Path
from dataclasses import dataclass, field
from typing import Set, List, Dict, Tuple, Optional
import argparse


@dataclass
class Function:
    idx: int
    name: str
    addr: int
    size: int
    is_game_logic: bool = False

    called_by: Set[str] = field(default_factory=set)
    calls: Set[str] = field(default_factory=set)

    classification: Optional[str] = None


def get_elf_info(libg: Path) -> Tuple[int, int, int]:
    result = subprocess.run(
        ["readelf", "-S", str(libg)],
        capture_output=True, text=True, check=True
    )
    for line in result.stdout.splitlines():
        m = re.match(
            r'\s*\[.*\]\s+\.text\s+PROGBITS\s+(\S+)\s+(\S+)\s+(\S+)',
            line
        )
        if m:
            return (int(m.group(1), 16), int(m.group(2), 16), int(m.group(3), 16))
    raise RuntimeError("Could not find .text section")


def get_all_symbols(libg: Path) -> Dict[str, Function]:
    result = subprocess.run(
        ["readelf", "-s", "--wide", str(libg)],
        capture_output=True, text=True, check=True
    )
    symbols = {}
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 8:
            continue
        try:
            idx = int(parts[0].rstrip(':'))
        except ValueError:
            continue
        typ = parts[2]
        bind = parts[4]
        ndx = parts[6]
        name = parts[7] if len(parts) > 7 else ""

        if typ == "FUNC" and bind == "GLOBAL" and ndx != "UND" and name:
            value = int(parts[1], 16)
            size = int(parts[2])
            symbols[name] = Function(idx=idx, name=name, addr=value, size=size)
    return symbols


def find_bl_targets_in_function(libg: Path, addr: int, size: int,
                                  text_offset: int, text_addr: int,
                                  symbols_by_addr: Dict[int, str]) -> Set[str]:
    """
    Scan ARM code for BL (Branch with Link) instructions and resolve targets.
    Returns set of symbol names called by this function.
    """
    if size == 0 or size > 500000:
        return set()

    file_offset = text_offset + (addr - text_addr)
    with open(libg, 'rb') as f:
        f.seek(file_offset)
        code = f.read(size)

    called = set()

    # ARM BL encoding: 0xEBxxxxxx where xxxxxx = offset[25:2]
    # PC in ARM is always word-aligned + 8 (due to pipeline)
    i = 0
    while i <= len(code) - 4:
        w = int.from_bytes(code[i:i+4], 'little')

        # Check for ARM BL/BLX (Branch with Link) pattern
        # ARM BL: bits [31:28]=1110, [27:25]=101, [24]=1
        # Pattern: 0xEB00xxxx to 0xEBFFFFFF
        if (w & 0xF8000000) == 0xE0000000 and (w & 0x01000000) == 0x01000000:
            # Extract 24-bit signed offset, shift right by 2
            offset = (w & 0x00FFFFFF)
            if offset & 0x00800000:
                offset |= 0xFF000000
            offset *= 4

            # PC is current address + 8 in ARM mode (pipeline)
            target = addr + i + 8 + offset

            if target in symbols_by_addr:
                called.add(symbols_by_addr[target])

        # Check for Thumb BL (we scan raw bytes, Thumb instructions vary)
        # Thumb BL (32-bit): patterns 0xF0xx / 0xF8xx high halfword
        # We check high halfword only for the pattern that indicates BL
        if i + 2 < len(code):
            h0 = code[i] | (code[i+1] << 8)
            if (h0 & 0xF000) == 0xF000:
                # Could be Thumb 32-bit BL encoding
                # Skip for now — this is complex
                pass

        i += 4

    return called


def classify_functions(functions: Dict[str, Function],
                       game_logic_names: Set[str]) -> None:
    for f in functions.values():
        if f.name in game_logic_names:
            f.is_game_logic = True

    for f in functions.values():
        if not f.is_game_logic:
            continue

        if not f.called_by:
            f.classification = "SAFE"
        elif all(
            caller in game_logic_names
            for caller in f.called_by
        ):
            f.classification = "SAFE"
        else:
            f.classification = "NEEDS_TRAMPOLINE"


def main():
    parser = argparse.ArgumentParser(description="Analyze ARM call graph")
    parser.add_argument("libg", type=Path, help="Path to original libg.so")
    parser.add_argument("symlist", type=Path, nargs="?", help="Game logic symbols file")
    args = parser.parse_args()

    libg = args.libg.resolve()
    print(f"Analyzing {libg}...")

    text_addr, text_offset, text_size = get_elf_info(libg)
    print(f".text: addr=0x{text_addr:08x}, offset=0x{text_offset:08x}, size=0x{text_size:08x}")

    symbols = get_all_symbols(libg)
    print(f"Total symbols: {len(symbols)}")

    game_logic_names = set()
    if args.symlist and args.symlist.exists():
        with open(args.symlist) as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                parts = line.split()
                if len(parts) >= 2:
                    game_logic_names.add(parts[1])

    symbols_by_addr = {f.addr: f.name for f in symbols.values() if f.addr}

    print(f"\nScanning all functions for BL calls...")
    count = 0
    for name, func in symbols.items():
        if func.size == 0 or func.size > 100000:
            continue

        called = find_bl_targets_in_function(
            libg, func.addr, func.size,
            text_offset, text_addr,
            symbols_by_addr
        )
        func.calls = called

        for callee_name in called:
            if callee_name in symbols:
                symbols[callee_name].called_by.add(name)

        count += 1
        if count % 2000 == 0:
            print(f"  {count}/{len(symbols)} functions...")

    print(f"  Done!")

    classify_functions(symbols, game_logic_names)

    safe = []
    needs_trampoline = []
    unknown = []

    for f in symbols.values():
        if not f.is_game_logic:
            continue
        if f.classification == "SAFE":
            safe.append(f)
        elif f.classification == "NEEDS_TRAMPOLINE":
            needs_trampoline.append(f)
        else:
            unknown.append(f)

    print(f"\n=== CLASSIFICATION ===")
    print(f"  SAFE (replace directly):          {len(safe)}")
    print(f"  NEEDS_TRAMPOLINE (ARM calls it):  {len(needs_trampoline)}")
    print(f"  UNKNOWN:                          {len(unknown)}")

    with open("call_graph_SAFE.txt", 'w') as f:
        f.write("# Functions safe to replace (no internal ARM callers)\n")
        f.write("# addr size name\n")
        for func in sorted(safe, key=lambda x: x.addr):
            f.write(f"0x{func.addr:08x} {func.size:8d} {func.name}\n")

    with open("call_graph_NEEDS_TRAMPOLINE.txt", 'w') as f:
        f.write("# Functions called by internal ARM — need branch trampoline\n")
        f.write("# addr size name\n")
        f.write("# callers: list\n\n")
        for func in sorted(needs_trampoline, key=lambda x: x.addr):
            f.write(f"0x{func.addr:08x} {func.size:8d} {func.name}\n")
            if func.called_by:
                callers = sorted(func.called_by)
                f.write(f"  called_by ({len(callers)}): {', '.join(callers[:5])}")
                if len(callers) > 5:
                    f.write(f" ... +{len(callers)-5} more")
                f.write("\n")
            f.write("\n")

    print(f"\nWrote call_graph_SAFE.txt ({len(safe)} functions)")
    print(f"Wrote call_graph_NEEDS_TRAMPOLINE.txt ({len(needs_trampoline)} functions)")

    print(f"\n=== TOP SAFE CANDIDATES (small, leaf functions) ===")
    candidates = [f for f in safe if 4 < f.size < 500 and len(f.calls) <= 2]
    for func in sorted(candidates, key=lambda x: x.size)[:20]:
        print(f"  0x{func.addr:08x} sz={func.size:5d} calls={len(func.calls):2d} {func.name}")


if __name__ == "__main__":
    main()
