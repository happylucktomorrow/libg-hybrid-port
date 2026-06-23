cmake_minimum_required(VERSION 3.22)
project(libg_hybrid_port LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Python3 REQUIRED)

# Path to original libg.so
set(ORIGINAL_LIBG "${CMAKE_SOURCE_DIR}/original/libg.so" CACHE PATH
    "Path to the original Boom Beach libg.so")

set(SYMBOLS_DIR "${CMAKE_SOURCE_DIR}/symbols")
set(INTERNAL_STUBS "${CMAKE_CURRENT_BINARY_DIR}/arm_exports.S")
set(TRAMPOLINES "${CMAKE_CURRENT_BINARY_DIR}/trampolines.S")

# Always generate ARM stubs (no compilation needed for this step)
add_custom_command(
    OUTPUT ${INTERNAL_STUBS}
    DEPENDS ${ORIGINAL_LIBG}
            ${CMAKE_SOURCE_DIR}/scripts/gen_arm_stubs.py
            ${SYMBOLS_DIR}/internal_symbols.txt
    COMMAND Python3::Interpreter
        ${CMAKE_SOURCE_DIR}/scripts/gen_arm_stubs.py
        ${ORIGINAL_LIBG}
        ${SYMBOLS_DIR}/internal_symbols.txt
        -o ${INTERNAL_STUBS}
    COMMENT "Generating ARM stubs for internal functions..."
    VERBATIM
)

add_custom_command(
    OUTPUT ${TRAMPOLINES}
    DEPENDS ${ORIGINAL_LIBG}
            ${CMAKE_SOURCE_DIR}/scripts/gen_trampolines.py
            ${SYMBOLS_DIR}/game_logic_symbols.txt
    COMMAND Python3::Interpreter
        ${CMAKE_SOURCE_DIR}/scripts/gen_trampolines.py
        ${ORIGINAL_LIBG}
        ${SYMBOLS_DIR}/game_logic_symbols.txt
        -o ${TRAMPOLINES}
    COMMENT "Generating trampolines for overridden game logic..."
    VERBATIM
)

add_custom_target(gen_stubs DEPENDS ${INTERNAL_STUBS} ${TRAMPOLINES})

# ============================================================================
# Android build (requires ANDROID_NDK_HOME set)
# ============================================================================
option(BUILD_ANDROID "Build Android libg.so" OFF)

if(BUILD_ANDROID AND NOT CMAKE_ANDROID_NDK)
    message(FATAL_ERROR "BUILD_ANDROID=ON but ANDROID_NDK_HOME not set. "
        "Set ANDROID_NDK_HOME to your Android NDK path, e.g.:\n"
        "  export ANDROID_NDK_HOME=/path/to/android-ndk-r25c\n"
        "  cmake -B build -DBUILD_ANDROID=ON ...")
endif()

if(BUILD_ANDROID AND CMAKE_ANDROID_NDK)
    message(STATUS "Building for Android: ${CMAKE_ANDROID_NDK}")

    # Internal ARM blob (generated stubs)
    add_library(arm_blob OBJECT ${INTERNAL_STUBS} ${TRAMPOLINES})

    # Game logic C++
    add_library(game_logic OBJECT
        src/game_logic/logic_commands.cpp
        src/game_logic/logic_state.cpp
        src/game_logic/logic_protocol.cpp
    )
    target_include_directories(game_logic PUBLIC include)

    # JNI bridge
    add_library(jni_bridge OBJECT
        src/jni/titan_jni.cpp
    )
    target_include_directories(jni_bridge PUBLIC include)

    # Hybrid shared library
    add_library(g SHARED
        $<TARGET_OBJECTS:arm_blob>
        $<TARGET_OBJECTS:game_logic>
        $<TARGET_OBJECTS:jni_bridge>
    )
    target_include_directories(g PRIVATE include)
    target_link_libraries(g PRIVATE android log EGL GLESv2 z)
    target_link_options(g PRIVATE -Wl,--allow-multiple-definition)
    set_target_properties(g PROPERTIES OUTPUT_NAME "g" ANDROID_OUTPUT_NAME "libg")

    message(STATUS "Built libg.so -> build/libg.so")
endif()

# ============================================================================
# Host analysis tools (always available)
# ============================================================================
option(BUILD_HOST_TOOLS "Build host-side analysis tools" OFF)

if(BUILD_HOST_TOOLS)
    enable_testing()
    find_package(ZLIB REQUIRED)

    # Just compile the game logic C++ for testing on host (no JNI)
    add_executable(logic_test
        tests/logic_test_host.cpp
        src/game_logic/logic_commands.cpp
        src/game_logic/logic_state.cpp
    )
    target_include_directories(logic_test PRIVATE include)
    target_link_libraries(logic_test PRIVATE ZLIB::ZLIB)
    add_test(NAME logic_test COMMAND logic_test)
endif()

message(STATUS "")
message(STATUS "=== libg-hybrid-port ===")
message(STATUS "  Original libg.so: ${ORIGINAL_LIBG}")
message(STATUS "  Symbols: ${SYMBOLS_DIR}/")
message(STATUS "")
message(STATUS "  For Android build:")
message(STATUS "    1. Install Android NDK (https://developer.android.com/ndk)")
message(STATUS "    2. Set: export ANDROID_NDK_HOME=/path/to/ndk")
message(STATUS "    3. Configure: cmake -B build -DBUILD_ANDROID=ON \\")
message(STATUS "                     -DCMAKE_TOOLCHAIN_FILE=\\$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake")
message(STATUS "    4. Build: cmake --build build")
message(STATUS "")
message(STATUS "  For live testing:")
message(STATUS "    1. adb push build/libg.so /data/data/com.supercell.boombeach/lib/libg.so")
message(STATUS "    2. Or repack into APK and reinstall")
message(STATUS "")
