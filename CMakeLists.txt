cmake_minimum_required(VERSION 3.22)
project(libg_hybrid_port LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Python3 REQUIRED)

# Path to original libg.so (override with -DORIGINAL_LIBG=...)
set(ORIGINAL_LIBG "${CMAKE_SOURCE_DIR}/original/libg.so" CACHE PATH
    "Path to the original Boom Beach libg.so")

# Generate ARM stubs for internal functions
set(SYMBOLS_DIR "${CMAKE_SOURCE_DIR}/symbols")
set(INTERNAL_STUBS "${CMAKE_CURRENT_BINARY_DIR}/arm_exports.S")
set(TRAMPOLINES "${CMAKE_CURRENT_BINARY_DIR}/trampolines.S")

# Custom commands to generate stubs from original libg.so
add_custom_command(
    OUTPUT ${INTERNAL_STUBS}
    DEPENDS ${ORIGINAL_LIBG}
            ${CMAKE_SOURCE_DIR}/scripts/gen_arm_stubs.py
    COMMAND Python3::Interpreter
        ${CMAKE_SOURCE_DIR}/scripts/gen_arm_stubs.py
        ${ORIGINAL_LIBG}
        ${SYMBOLS_DIR}/internal_symbols.txt
        -o ${INTERNAL_STUBS}
    COMMENT "Generating ARM stubs for internal functions..."
)

add_custom_command(
    OUTPUT ${TRAMPOLINES}
    DEPENDS ${ORIGINAL_LIBG}
            ${CMAKE_SOURCE_DIR}/scripts/gen_trampolines.py
    COMMAND Python3::Interpreter
        ${CMAKE_SOURCE_DIR}/scripts/gen_trampolines.py
        ${ORIGINAL_LIBG}
        ${SYMBOLS_DIR}/game_logic_symbols.txt
        -o ${TRAMPOLINES}
    COMMENT "Generating trampolines for overridden game logic..."
)

# Internal ARM blob library (original code + trampolines)
add_library(arm_blob OBJECT ${INTERNAL_STUBS} ${TRAMPOLINES})
set_source_files_properties(
    ${INTERNAL_STUBS} ${TRAMPOLINES}
    PROPERTIES COMPILE_OPTIONS "-x assembler-with-cpp"
)

# Game logic C++ library
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

if(CMAKE_ANDROID_NDK)
    target_link_libraries(g PRIVATE android log EGL GLESv2 z)
else()
    target_link_libraries(g PRIVATE z)
endif()

target_link_options(g PRIVATE
    -Wl,--allow-multiple-definition
    -Wl,--no-undefined
)

set_target_properties(g PROPERTIES OUTPUT_NAME "g")

# Host-side tests (optional)
option(BUILD_HOST_TESTS "Build host-side tests" ON)
if(BUILD_HOST_TESTS AND NOT CMAKE_ANDROID_NDK)
    enable_testing()

    add_library(test_tweetnacl STATIC
        third_party/tweetnacl/randombytes.c
        third_party/tweetnacl/tweetnacl.c
    )
    target_include_directories(test_tweetnacl PUBLIC third_party/tweetnacl)

    function(add_hybrid_test name)
        add_executable(${name} ${ARGN})
        target_include_directories(${name} PRIVATE include third_party/tweetnacl)
        target_link_libraries(${name} PRIVATE test_tweetnacl z)
        add_test(NAME ${name} COMMAND ${name})
    endfunction()

    add_hybrid_test(logic_commands_test
        tests/logic_commands_test.cpp
        src/game_logic/logic_commands.cpp
    )
endif()

# Convenience target to regenerate stubs
add_custom_target(gen_stubs
    DEPENDS ${INTERNAL_STUBS} ${TRAMPOLINES}
)
