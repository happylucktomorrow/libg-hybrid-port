#include "game/libg_api.h"

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <cstdio>
#include <cstring>
#include <string>

// ============================================================================
// JNI bridge — Java_com_supercell_titan_* entry points
// These match the original libg.so's exported JNI symbols exactly.
// ============================================================================

namespace {

// Forward declarations for game logic functions we call from JNI
void game_main_init(int width, int height);
void game_main_deinit();
bool game_main_back_button_pressed();
void game_main_clear_touches();
void game_main_surface_created();
void game_main_surface_changed(int width, int height);
void game_main_draw_frame();
void game_main_touch_event(int action, float x, float y, int pointer_id);
void game_main_key_event(int key_code, int action);
void game_main_text_input(const char* text);
void game_main_on_pause();
void game_main_on_resume();
void game_main_on_low_memory();

std::string jstring_to_string(JNIEnv* env, jstring value) {
    if (!value) return {};
    const char* chars = env->GetStringUTFChars(value, nullptr);
    if (!chars) return {};
    std::string result(chars);
    env->ReleaseStringUTFChars(value, chars);
    return result;
}

} // anonymous namespace

// ============================================================================
// JNI_OnLoad / JNI_OnUnload
// ============================================================================

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
}

// ============================================================================
// GameApp lifecycle
// ============================================================================

extern "C" JNIEXPORT jboolean JNICALL
Java_com_supercell_titan_GameApp_backButtonPressed(JNIEnv*, jclass) {
    return game_main_back_button_pressed() ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_clearTouches(JNIEnv*, jclass) {
    game_main_clear_touches();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_supercell_titan_GameApp_createGameMain(
    JNIEnv* env, jclass,
    jobject asset_manager,
    jstring home_dir,
    jstring cache_dir,
    jstring apk_path,
    jlong, jint width, jint height, jint, jint)
{
    game_main_init(width, height);
    return env->NewStringUTF("");
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_deinit(JNIEnv*, jclass) {
    game_main_deinit();
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onCreate(JNIEnv*, jclass) {
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onDestroy(JNIEnv*, jclass) {
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onLowMemory(JNIEnv*, jclass) {
    game_main_on_low_memory();
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onPause(JNIEnv*, jclass) {
    game_main_on_pause();
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onResume(JNIEnv*, jclass) {
    game_main_on_resume();
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_surfaceCreated(JNIEnv*, jclass) {
    game_main_surface_created();
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_surfaceChanged(JNIEnv*, jclass,
    jint width, jint height) {
    game_main_surface_changed(width, height);
}

// ============================================================================
// Touch / Input
// ============================================================================

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onTouchEvent(JNIEnv*, jclass,
    jint action, jfloat x, jfloat y, jint pointer_id) {
    game_main_touch_event(action, x, y, pointer_id);
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onKeyEvent(JNIEnv*, jclass,
    jint key_code, jint action) {
    game_main_key_event(key_code, action);
}

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_onTextInput(JNIEnv* env, jclass,
    jstring text) {
    game_main_text_input(jstring_to_string(env, text).c_str());
}

// ============================================================================
// Frame rendering
// ============================================================================

extern "C" JNIEXPORT void JNICALL
Java_com_supercell_titan_GameApp_drawFrame(JNIEnv*, jclass) {
    game_main_draw_frame();
}

// ============================================================================
// Titan NativeBridge / misc
// ============================================================================

extern "C" JNIEXPORT jstring JNICALL
Java_com_supercell_titan_NativeBridge_nativeGetUserAgentString(JNIEnv* env, jclass) {
    return env->NewStringUTF("libg-hybrid-port");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_supercell_titan_NativeBridge_nativeGetAdvertisingIdString(JNIEnv* env, jclass) {
    return env->NewStringUTF("");
}
