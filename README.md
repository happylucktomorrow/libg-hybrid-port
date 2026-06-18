# libg-hybrid-port

**Hybrid port of Boom Beach `libg.so`** — original ARM code for internal/framework functions + clean C++ for public game logic.

## Motivation

The companion project [`libg-original-reconstruction`](https://github.com/happylucktomorrow/libg-original-reconstruction) aims for byte-for-byte faithful decompilation of the entire library. That's ideal for preservation but overkill if you only need to **modify game logic**.

This project takes a different approach:

| Component | Approach | Why |
|-----------|----------|-----|
| Display framework, renderer, animation, UI screens, etc. | **Original ARM blob** (extracted from `libg.so`) | These don't need changing; keep them as-is |
| Game logic classes (`Logic*`, commands, protocol, crypto, game state) | **Clean C++** | These contain the rules you want to customize |
| JNI bridge | **Clean C++** | Must match Java expectations exactly |

## Architecture

```
Original libg.so (ARM32)
  │
  ├── scripts/classify_symbols.py    → Symbol classification
  ├── scripts/gen_arm_stubs.py       → ARM assembly stubs (internal funcs)
  ├── scripts/gen_trampolines.py     → Trampolines for overridden game logic
  │
  ├── src/internal/                  → ARM assembly with embedded code
  │   └── arm_exports.S (.incbin slices of original .text)
  │
  ├── src/game_logic/                → Your C++ game logic
  │   ├── logic_commands.cpp
  │   ├── logic_state.cpp
  │   └── logic_protocol.cpp
  │
  ├── src/jni/                       → JNI bridge
  │
  └── libg.so (output)               → Hybrid library ready for Android
```

### How function overriding works

When you replace a game logic function with new C++ code, calls to it from the ARM blob must be redirected. This is handled by **trampolines**:

1. The original ARM code at the function's address is replaced with a trampoline (branch to our C++ implementation)
2. The trampoline is exactly 4–8 bytes (one or two ARM instructions)
3. All internal BL instructions in the ARM blob still target the same offset — the trampoline sits at that offset

This keeps all original ARM-internal call offsets valid while redirecting execution.

## Getting Started

### Prerequisites

- Android NDK r25+ (or any ARM32 cross-compiler)
- Python 3.10+
- The original Boom Beach APK (or `libg.so`)

### Setup

```bash
# Symlink the original library
ln -s /path/to/original/libg.so original/libg.so

# Classify symbols
python scripts/classify_symbols.py original/libg.so

# Generate ARM stubs for internal functions
python scripts/gen_arm_stubs.py original/libg.so symbols/internal_symbols.txt

# Generate trampolines for overridden game logic
python scripts/gen_trampolines.py original/libg.so symbols/game_logic_symbols.txt

# Build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-19
cmake --build build
```

## Symbol Classification

Symbols are classified as **game logic** (port to C++) if their mangled name matches:

- `Logic*` classes (game logic, commands, components, data)
- Protocol message classes (`*Message`)
- Protocol/crypto functions
- Game state classes (`HomeState`, `GameData`, `StaticData`, etc.)
- Avatar/player management
- JNI entry points

All other symbols (display framework, renderer, audio, animation, tweening, screens, UI, etc.) are kept as **internal ARM blobs**.

## Project Structure

```
libg-hybrid-port/
├── original/                → Original libg.so binary
├── scripts/                 → Analysis and code generation
├── symbols/                 → Classified symbol lists
├── src/
│   ├── game_logic/          → C++ game logic implementations
│   ├── internal/            → ARM assembly stubs + trampolines
│   └── jni/                 → JNI bridge (Java_com_supercell_titan_*)
├── include/                 → Public headers
└── CMakeLists.txt
```
