#!/usr/bin/env python3
"""
Classify exported symbols from the original libg.so as:
  - Game logic (port to C++)
  - Internal/framework (keep as ARM blob)

Outputs two files:
  symbols/game_logic_symbols.txt
  symbols/internal_symbols.txt
"""

import subprocess
import sys
import re
from pathlib import Path

GAME_LOGIC_PATTERNS = [
    r'^_ZN5Logic',           # Logic* classes
    r'^_ZN.*Logic\w+',       # Any Logic-prefixed
    r'^_ZN.*Command',        # Command classes
    r'^_ZN.*Message',        # Protocol messages
    r'^_ZN.*HomeState',      # Home state
    r'^_ZN.*GameData',       # Game data
    r'^_ZN.*StaticData',     # Static data
    r'^_ZN.*GameMain',       # Game main
    r'^_ZN.*Protocol',       # Protocol
    r'^_ZN.*Codec',          # Codec
    r'^_ZN.*CryptoSession',  # Crypto
    r'^_ZN.*Avatar',         # Avatar
    r'^_ZN.*PlayerMap',      # Player map
    r'^_ZN.*Battle\w*Test',  # Battle tests
    r'^_ZN.*HomeCommands',   # Home commands
    r'^_ZN.*BattleCommands', # Battle commands
    r'^Java_com_supercell',  # JNI exports
]

INTERNAL_PATTERNS = [
    r'^_ZN.*Display',        # Display framework
    r'^_ZN.*MovieClip',      # MovieClip
    r'^_ZN.*Sprite',         # Sprite
    r'^_ZN.*TextField',      # Text field
    r'^_ZN.*Stage',          # Stage
    r'^_ZN.*ISO',            # Isometric
    r'^_ZN.*Iso',            # Iso classes
    r'^_ZN.*HUD',            # HUD
    r'^_ZN.*Screen',         # Screens
    r'^_ZN.*Menu',           # Menus
    r'^_ZN.*Popover',        # Popovers
    r'^_ZN.*Button',         # Buttons
    r'^_ZN.*Tween',          # Tweening
    r'^_ZN.*Anim',           # Animation
    r'^_ZN.*Sound',          # Audio
    r'^_ZN.*Particle',       # Particles
    r'^_ZN.*Render',         # Render
    r'^_ZN.*Shader',         # Shaders
    r'^_ZN.*Texture',        # Textures
    r'^_ZN.*Bundle',         # SC bundles
    r'^_ZN.*Titan',          # Titan framework
    r'^_ZN.*Sparky',         # Sparky engine
    r'^_ZN.*Base',           # Base classes
    r'^_ZN.*UI\w+',          # UI
    r'^std::',               # STL
    r'^__cxa',               # C++ ABI
    r'^_Unwind',             # Unwind
    r'^icu::',               # ICU
    r'^u_charCategory',      # ICU C
    r'^u_getIntPropertyValue',
    r'^u_isspace',
    r'^u_is',
    r'^u_str',
    r'^tweetnacl',
    r'^randombytes',
]


def is_game_logic(name: str) -> bool:
    for p in GAME_LOGIC_PATTERNS:
        if re.match(p, name):
            return True
    return False


def is_internal(name: str) -> bool:
    for p in INTERNAL_PATTERNS:
        if re.match(p, name):
            return True
    return False


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <libg.so> [outdir]")
        sys.exit(1)

    libg = Path(sys.argv[1])
    outdir = Path(sys.argv[2]) if len(sys.argv) > 2 else Path("symbols")
    outdir.mkdir(parents=True, exist_ok=True)

    result = subprocess.run(
        ["readelf", "-s", "--wide", str(libg)],
        capture_output=True, text=True, check=True
    )

    game_logic = []
    internal = []
    unclassified = []

    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 8:
            continue
        try:
            idx = int(parts[0].rstrip(':'))
        except ValueError:
            continue
        bind = parts[4]
        ndx = parts[6]
        name = parts[7] if len(parts) > 7 else ""

        if bind != "GLOBAL" or ndx == "UND":
            continue
        if not name or name.startswith("_ZTV") or name.startswith("_ZTI"):
            continue

        if is_game_logic(name):
            game_logic.append((idx, name))
        elif is_internal(name):
            internal.append((idx, name))
        else:
            unclassified.append((idx, name))

    def write_list(path, items, label):
        with open(path, 'w') as f:
            f.write(f"# {label} symbols ({len(items)} total)\n")
            f.write("# format: <elf_idx> <mangled_name>\n")
            for idx, name in items:
                f.write(f"{idx} {name}\n")
        print(f"Wrote {len(items)} {label} symbols to {path}")

    write_list(outdir / "game_logic_symbols.txt", game_logic, "Game logic")
    write_list(outdir / "internal_symbols.txt", internal, "Internal")
    write_list(outdir / "unclassified_symbols.txt", unclassified, "Unclassified")

    total = len(game_logic) + len(internal) + len(unclassified)
    print(f"\nTotal GLOBAL (non-vtable) symbols: {total}")
    print(f"  Game logic:  {len(game_logic)} ({100*len(game_logic)//total}%)")
    print(f"  Internal:    {len(internal)} ({100*len(internal)//total}%)")
    print(f"  Unclassified: {len(unclassified)} ({100*len(unclassified)//total}%)")


if __name__ == "__main__":
    main()
