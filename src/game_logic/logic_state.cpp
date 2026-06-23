#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>

// ============================================================================
// Game state management
// Replaces LogicGameObjectManager, LogicCommandManager, GameState, etc.
// ============================================================================

// Global game state (singleton-like, accessed via original symbol names)
namespace {

struct GameState {
    LogicClientAvatar* avatar = nullptr;
    LogicClientHome* home = nullptr;
    LogicGameObjectManager* obj_mgr = nullptr;
    LogicCommandManager* cmd_mgr = nullptr;
    bool initialized = false;
};

GameState g_state;
std::unordered_map<int32_t, void*> g_objects_by_id;
std::unordered_map<void*, int32_t> g_id_by_object;
std::vector<void*> g_command_history;
int32_t g_next_object_id = 1;

} // anonymous namespace

// ============================================================================
// LogicGameObjectManager
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN22LogicGameObjectManagerC1Ev(void* self) {
    // Constructor - initialize object manager
    memset(self, 0, 64);
    if (!g_state.obj_mgr) {
        g_state.obj_mgr = static_cast<LogicGameObjectManager*>(self);
    }
}

extern "C" LIBG_EXPORT
void _ZN22LogicGameObjectManagerD1Ev(void* self) {
    // Destructor - cleanup
}

extern "C" LIBG_EXPORT
void _ZN22LogicGameObjectManager14addGameObjectEPN5Logic12GameObjectE(void* self, void* obj) {
    if (!obj) {
        return;
    }

    const int32_t id = g_next_object_id++;
    g_objects_by_id[id] = obj;
    g_id_by_object[obj] = id;
}

extern "C" LIBG_EXPORT
void _ZN22LogicGameObjectManager17removeGameObjectEPN5Logic12GameObjectE(void* self, void* obj) {
    if (!obj) {
        return;
    }

    const auto it = g_id_by_object.find(obj);
    if (it == g_id_by_object.end()) {
        return;
    }

    g_objects_by_id.erase(it->second);
    g_id_by_object.erase(it);
}

extern "C" LIBG_EXPORT
void* _ZN22LogicGameObjectManager13getGameObjectEi(void* self, int32_t id) {
    const auto it = g_objects_by_id.find(id);
    return it == g_objects_by_id.end() ? nullptr : it->second;
}

extern "C" LIBG_EXPORT
int32_t _ZN22LogicGameObjectManager11getObjectIdEv(void* self) {
    // Generate next unique object ID
    static int32_t next_id = 1;
    return next_id++;
}

// ============================================================================
// LogicCommandManager
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN18LogicCommandManagerC1Ev(void* self) {
    memset(self, 0, 32);
    if (!g_state.cmd_mgr) {
        g_state.cmd_mgr = static_cast<LogicCommandManager*>(self);
    }
}

extern "C" LIBG_EXPORT
void _ZN18LogicCommandManagerD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
bool _ZN18LogicCommandManager11executeCommandEPN5Logic7CommandE(void* self, void* cmd) {
    if (!cmd) {
        return false;
    }

    g_command_history.push_back(cmd);
    return true;
}

extern "C" LIBG_EXPORT
void _ZN18LogicCommandManager8undoLastEv(void* self) {
    if (!g_command_history.empty()) {
        g_command_history.pop_back();
    }
}

// ============================================================================
// LogicClientAvatar
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN17LogicClientAvatarC1Ev(void* self) {
    memset(self, 0, 256);
    if (!g_state.avatar) {
        g_state.avatar = static_cast<LogicClientAvatar*>(self);
    }
}

extern "C" LIBG_EXPORT
void _ZN17LogicClientAvatarD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
int32_t _ZN17LogicClientAvatar8getScoreEv(void* self) {
    // Return player's VP/score
    return 0;
}

extern "C" LIBG_EXPORT
int32_t _ZN17LogicClientAvatar10getDiamondsEv(void* self) {
    // Return player's diamond count
    return 0;
}

extern "C" LIBG_EXPORT
int32_t _ZN17LogicClientAvatar9getLevelsEv(void* self) {
    // Return player's XP level
    return 1;
}

// ============================================================================
// LogicClientHome
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN14LogicClientHomeC1Ev(void* self) {
    memset(self, 0, 1024);
    if (!g_state.home) {
        g_state.home = static_cast<LogicClientHome*>(self);
    }
}

extern "C" LIBG_EXPORT
void _ZN14LogicClientHomeD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
bool _ZN14LogicClientHome7isMineEv(void* self) {
    return true;
}
