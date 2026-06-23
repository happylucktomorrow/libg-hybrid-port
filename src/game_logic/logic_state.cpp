#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
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

struct OpaqueGameObjectHeader {
    int32_t object_id;
};

GameState g_state;
std::unordered_map<int32_t, void*> g_objects_by_id;
std::unordered_map<void*, int32_t> g_id_by_object;
int32_t g_next_object_id = 1;
std::vector<void*> g_pending_commands;
ResourceWallet g_wallet{10000, 10000, 10000, 10000, 100};
int32_t g_avatar_level = 1;
int32_t g_avatar_score = 0;

} // anonymous namespace

extern "C" LIBG_EXPORT
void* libg_get_game_object_by_id(int32_t id) {
    const auto it = g_objects_by_id.find(id);
    return it == g_objects_by_id.end() ? nullptr : it->second;
}

extern "C" LIBG_EXPORT
void libg_add_game_object(void* obj) {
    if (!obj) {
        return;
    }
    const int32_t id = g_next_object_id++;
    g_objects_by_id[id] = obj;
    g_id_by_object[obj] = id;
    auto* header = static_cast<OpaqueGameObjectHeader*>(obj);
    header->object_id = id;
}

extern "C" LIBG_EXPORT
void libg_remove_game_object(void* obj) {
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
void libg_register_command(void* cmd) {
    if (cmd) {
        g_pending_commands.push_back(cmd);
    }
}

extern "C" LIBG_EXPORT
int32_t libg_get_wallet_amount(int32_t resource_type) {
    switch (static_cast<ResourceType>(resource_type)) {
    case ResourceType::Gold: return g_wallet.gold;
    case ResourceType::Wood: return g_wallet.wood;
    case ResourceType::Stone: return g_wallet.stone;
    case ResourceType::Iron: return g_wallet.iron;
    case ResourceType::Diamonds: return g_wallet.diamonds;
    default: return 0;
    }
}

extern "C" LIBG_EXPORT
bool libg_spend_wallet_amount(int32_t resource_type, int32_t amount) {
    if (amount < 0) {
        return false;
    }
    const auto type = static_cast<ResourceType>(resource_type);
    if (!g_wallet.can_afford(type, amount)) {
        return false;
    }
    g_wallet.spend(type, amount);
    return true;
}

extern "C" LIBG_EXPORT
void libg_add_wallet_amount(int32_t resource_type, int32_t amount) {
    if (amount <= 0) {
        return;
    }
    g_wallet.add(static_cast<ResourceType>(resource_type), amount);
}

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
    g_state.initialized = true;
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

    const auto existing = g_id_by_object.find(obj);
    if (existing != g_id_by_object.end()) {
        return;
    }

    const int32_t id = g_next_object_id++;
    g_objects_by_id[id] = obj;
    g_id_by_object[obj] = id;

    auto* header = static_cast<OpaqueGameObjectHeader*>(obj);
    header->object_id = id;
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
    return g_next_object_id++;
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
void _ZN19LogicCommandManagerD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager8destructEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManagerC1EP10LogicLevel(void* self, void* level) {
    _ZN18LogicCommandManagerC1Ev(self);
    if (self) {
        std::memcpy(static_cast<uint8_t*>(self) + 0x14, &level, sizeof(level));
    }
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManagerC2EP10LogicLevel(void* self, void* level) {
    _ZN19LogicCommandManagerC1EP10LogicLevel(self, level);
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager10addCommandEP12LogicCommand(void* self, void* cmd) {
    (void)self;
    libg_register_command(cmd);
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager11setListenerEP27LogicCommandManagerListener(void* self, void* listener) {
    (void)self;
    (void)listener;
}

extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(void*, void*, int32_t, int32_t, bool);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommandC1Eiii(void*, int32_t, int32_t, int32_t);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommandC1Eib(void*, int32_t, bool);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommandC1Ei(void*, int32_t);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommandC1Ei(void*, int32_t);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(void*, int32_t, void*);
extern "C" LIBG_EXPORT void _ZN31LogicSpeedUpConstructionCommandC1Ei(void*, int32_t);
extern "C" LIBG_EXPORT void _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(void*, void*);

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager13createCommandEi(void* self, int32_t type) {
    (void)self;
    switch (type) {
    case 500: {
        void* cmd = std::calloc(1, 0x20);
        if (cmd) {
            _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(cmd, nullptr, 0, 0, false);
        }
        return cmd;
    }
    case 0x1f5: {
        void* cmd = std::calloc(1, 0x20);
        if (cmd) {
            _ZN24LogicMoveBuildingCommandC1Eiii(cmd, 0, 0, 0);
        }
        return cmd;
    }
    case 0x1f6: {
        void* cmd = std::calloc(1, 0x20);
        if (cmd) {
            _ZN27LogicUpgradeBuildingCommandC1Eib(cmd, 0, false);
        }
        return cmd;
    }
    case 0x1f7: {
        void* cmd = std::calloc(1, 0x18);
        if (cmd) {
            _ZN24LogicSellBuildingCommandC1Ei(cmd, 0);
        }
        return cmd;
    }
    case 0x1f8: {
        void* cmd = std::calloc(1, 0x10);
        if (cmd) {
            _ZN31LogicSpeedUpConstructionCommandC1Ei(cmd, 0);
        }
        return cmd;
    }
    case 0x1fb: {
        void* cmd = std::calloc(1, 0x30);
        if (cmd) {
            _ZN25LogicClearObstacleCommandC1Ei(cmd, 0);
        }
        return cmd;
    }
    case 0x1fc: {
        void* cmd = std::calloc(1, 0x20);
        if (cmd) {
            _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(cmd, 0, nullptr);
        }
        return cmd;
    }
    case 0x24f: {
        void* cmd = std::calloc(1, 0x18);
        if (cmd) {
            _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(cmd, nullptr);
        }
        return cmd;
    }
    default:
        return nullptr;
    }
}

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager13decodeCommandEP10ByteStreambb(void* self, void* stream, bool a, bool b) {
    (void)self;
    (void)stream;
    (void)a;
    (void)b;
    return nullptr;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager13encodeCommandEP15ChecksumEncoderP12LogicCommand(void* self, void* encoder, void* cmd) {
    (void)self;
    (void)encoder;
    (void)cmd;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager17saveCommandToJSONEP15LogicJSONObjectP12LogicCommand(void* self, void* json, void* cmd) {
    (void)self;
    (void)json;
    (void)cmd;
}

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager19loadCommandFromJSONEP15LogicJSONObject(void* self, void* json) {
    (void)self;
    (void)json;
    return nullptr;
}

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager6decodeEP10ByteStream(void* self, void* stream) {
    (void)self;
    (void)stream;
    return nullptr;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    (void)self;
    (void)encoder;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager7subTickEv(void* self) {
    (void)self;
}

extern "C" LIBG_EXPORT
void _ZNK19LogicCommandManager30isCommandAllowedInCurrentStateEP12LogicCommand(void* self, void* cmd) {
    if (!self || !cmd) {
        return;
    }

    const int32_t type = *reinterpret_cast<int32_t*>(cmd);
    const void* level = *reinterpret_cast<void* const*>(static_cast<uint8_t*>(self) + 0x14);
    if (!level) {
        return;
    }
    switch (type) {
    case 500:
    case 0x1f5:
    case 0x1f6:
    case 0x1f7:
    case 0x1f8:
    case 0x1fb:
    case 0x1fc:
    case 0x24f:
        break;
    default:
        break;
    }
}

extern "C" LIBG_EXPORT
void _ZN27LogicCommandManagerListenerC1Ev(void* self) {
    if (self) {
        std::memset(self, 0, 16);
    }
}

extern "C" LIBG_EXPORT
void _ZN27LogicCommandManagerListenerC2Ev(void* self) {
    _ZN27LogicCommandManagerListenerC1Ev(self);
}

extern "C" LIBG_EXPORT
void _ZN27LogicCommandManagerListener8destructEv(void* self) {
    (void)self;
}

extern "C" LIBG_EXPORT
void _ZN27LogicCommandManagerListener15commandExecutedEP12LogicCommand(void* self, void* cmd) {
    (void)self;
    (void)cmd;
}

extern "C" LIBG_EXPORT
bool _ZN18LogicCommandManager11executeCommandEPN5Logic7CommandE(void* self, void* cmd) {
    (void)self;
    (void)cmd;
    return true;
}

extern "C" LIBG_EXPORT
void _ZN18LogicCommandManager8undoLastEv(void* self) {
    (void)self;
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
    (void)self;
    return g_avatar_score;
}

extern "C" LIBG_EXPORT
int32_t _ZN17LogicClientAvatar10getDiamondsEv(void* self) {
    (void)self;
    return g_wallet.diamonds;
}

extern "C" LIBG_EXPORT
int32_t _ZN17LogicClientAvatar9getLevelsEv(void* self) {
    (void)self;
    return g_avatar_level;
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
