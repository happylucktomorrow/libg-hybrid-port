#!/bin/bash
# Convenience script to build libg-hybrid-port
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_DIR"

# Check for original libg.so
if [ ! -f original/libg.so ]; then
    echo "Error: original/libg.so not found."
    echo "Please symlink or copy the original Boom Beach libg.so:"
    echo "  ln -s /path/to/client_apktool/lib/armeabi-v7a/libg.so original/libg.so"
    exit 1
fi

# Check for NDK
if [ -z "${ANDROID_NDK_HOME:-}" ]; then
    if [ -n "${ANDROID_HOME:-}" ]; then
        export ANDROID_NDK_HOME="$ANDROID_HOME/ndk/$(ls -1 "$ANDROID_HOME/ndk" 2>/dev/null | sort -V | tail -1)"
    fi
fi

# Step 1: Classify symbols
echo "=== Classifying symbols ==="
python3 scripts/classify_symbols.py original/libg.so

# Step 2: Configure and build
echo "=== Building ==="
if [ -n "${ANDROID_NDK_HOME:-}" ]; then
    cmake --preset android-armeabi-v7a \
        -DORIGINAL_LIBG="$PWD/original/libg.so"
    cmake --build --preset android-armeabi-v7a
else
    echo "ANDROID_NDK_HOME not set — building for host only"
    cmake --preset host \
        -DORIGINAL_LIBG="$PWD/original/libg.so"
    cmake --build --preset host
    ctest --preset host
fi

echo "=== Done ==="
