#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <type_traits>

extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void* _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN31LogicSpeedUpConstructionCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN31LogicSpeedUpConstructionCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void* _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN24LogicBuyResourcesCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN24LogicBuyResourcesCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN12LogicCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN19LogicCommandManager11setListenerEP27LogicCommandManagerListener(void* self, void* listener);
extern "C" LIBG_EXPORT void _ZN27LogicCommandManagerListenerC1Ev(void* self);
extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN31LogicSpeedUpConstructionCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN24LogicBuyResourcesCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);

// ============================================================================
// Game state management
// Replaces LogicGameObjectManager, LogicCommandManager, GameState, etc.
// ============================================================================

// Global game state (singleton-like, accessed via original symbol names)
namespace {

template <typename T>
T load_at(const void* obj, std::size_t offset) {
    T out{};
    if (obj) {
        std::memcpy(&out, static_cast<const uint8_t*>(obj) + offset, sizeof(out));
    }
    return out;
}

template <typename T>
void store_at(void* obj, std::size_t offset, T value) {
    if (obj) {
        std::memcpy(static_cast<uint8_t*>(obj) + offset, &value, sizeof(value));
    }
}

template <typename Ret, typename... Args>
Ret call_vmethod(void* obj, std::size_t byte_offset, Args... args) {
    if (!obj) {
        if constexpr (!std::is_void_v<Ret>) {
            return Ret{};
        } else {
            return;
        }
    }
    auto** vtable = *reinterpret_cast<void***>(obj);
    using Fn = Ret (*)(void*, Args...);
    auto fn = reinterpret_cast<Fn>(vtable[byte_offset / sizeof(void*)]);
    if constexpr (std::is_void_v<Ret>) {
        fn(obj, args...);
    } else {
        return fn(obj, args...);
    }
}

inline void write_u32(void* encoder, uint32_t value) {
    if (encoder) {
        call_vmethod<void>(encoder, 0x1c, value);
    }
}

inline uint32_t read_u32(void* stream) {
    return stream ? call_vmethod<uint32_t>(stream, 0x54) : 0;
}

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

struct LogicCommandManagerListenerVTable {
    void (*destruct)(void*);
    void (*delete_self)(void*);
};

inline int32_t command_type(void* cmd);

void destroy_known_command(void* cmd) {
    if (!cmd) {
        return;
    }
    switch (command_type(cmd)) {
    case 500: _ZN23LogicBuyBuildingCommand8destructEv(cmd); break;
    case 0x1f5: _ZN24LogicMoveBuildingCommand8destructEv(cmd); break;
    case 0x1f6: _ZN27LogicUpgradeBuildingCommand8destructEv(cmd); break;
    case 0x1f7: _ZN24LogicSellBuildingCommand8destructEv(cmd); break;
    case 0x1f8: _ZN31LogicSpeedUpConstructionCommand8destructEv(cmd); break;
    case 0x1fb: _ZN25LogicClearObstacleCommand8destructEv(cmd); break;
    case 0x1fc: _ZN21LogicTrainUnitCommand8destructEv(cmd); break;
    case 0x24f: _ZN24LogicBuyResourcesCommand8destructEv(cmd); break;
    default: _ZN12LogicCommand8destructEv(cmd); break;
    }
    std::free(cmd);
}

inline int32_t command_type(void* cmd) {
    return cmd ? *reinterpret_cast<int32_t*>(cmd) : 0;
}

void dispatch_command(void* cmd, void* level) {
    if (!cmd || !level) {
        return;
    }
    switch (command_type(cmd)) {
    case 500: _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x1f5: _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x1f6: _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x1f7: _ZN24LogicSellBuildingCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x1f8: _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x1fb: _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x1fc: _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(cmd, level); break;
    case 0x24f: _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(cmd, level); break;
    default: break;
    }
}

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

extern "C" LIBG_EXPORT
int32_t _ZN22LogicGameObjectManager24getGameObjectCountByDataEP8LogicData(void* self, void* data) {
    (void)self;
    if (!data) {
        return 0;
    }
    int32_t count = 0;
    for (const auto& entry : g_objects_by_id) {
        void* obj = entry.second;
        if (!obj) {
            continue;
        }
        if (call_vmethod<void*>(obj, 0x58) == data) {
            ++count;
        }
    }
    return count;
}

extern "C" LIBG_EXPORT
void* _ZNK10LogicLevel13getPlayerAvatarEv(void* self) {
    if (!self) {
        return nullptr;
    }
    void* game_mode = load_at<void*>(self, 0x40);
    const int32_t state = load_at<int32_t>(game_mode, 4);
    const std::size_t offset = (state == 1 || state == 3) ? 0x38 : 0x3c;
    return load_at<void*>(self, offset);
}

extern "C" LIBG_EXPORT
int32_t _ZNK18LogicWorkerManager14getFreeWorkersEv(void* self) {
    if (!self) {
        return 0;
    }
    return load_at<int32_t>(self, 0x14) - load_at<int32_t>(self, 0x10);
}

extern "C" LIBG_EXPORT
bool _ZNK17LogicClientAvatar18hasEnoughResourcesEPK19LogicResourceBundlebP12LogicCommand(void* self, void* bundle, bool notify, void* cmd) {
    if (!self || !bundle) {
        return false;
    }
    bool enough = true;
    for (int i = 0; i < 5; ++i) {
        const int32_t amount = load_at<int32_t>(bundle, 4 + i * 4);
        if (amount <= 0) {
            continue;
        }
        void* data = load_at<void*>(bundle, 0x18 + i * 4);
        const int32_t have = call_vmethod<int32_t>(self, 0x28, data);
        enough = enough && (amount <= have);
    }
    if (!enough && notify) {
        void* level = load_at<void*>(self, 0x44);
        if (level) {
            void* listener = load_at<void*>(level, 0x2c);
            if (listener) {
                call_vmethod<void>(listener, 0x10, bundle, cmd);
            }
        }
    }
    return enough;
}

extern "C" LIBG_EXPORT
bool _ZNK17LogicClientAvatar18hasEnoughResourcesEP17LogicResourceDataibP12LogicCommand(void* self, void* data, int32_t amount, bool notify, void* cmd) {
    if (!self || !data) {
        return false;
    }
    const int32_t have = call_vmethod<int32_t>(self, 0x28, data);
    const bool enough = amount <= have;
    if (!enough && notify) {
        void* level = load_at<void*>(self, 0x44);
        if (level) {
            void* listener = load_at<void*>(level, 0x2c);
            if (listener) {
                call_vmethod<void>(listener, 0x0c, data, amount, cmd);
            }
        }
    }
    return enough;
}

extern "C" LIBG_EXPORT
int32_t _ZN10LogicLevel14getArtifactCountEv(void* self) {
    (void)self;
    int32_t count = 0;
    for (const auto& entry : g_objects_by_id) {
        void* obj = entry.second;
        if (!obj) {
            continue;
        }
        void* data = call_vmethod<void*>(obj, 0x58);
        if (data && load_at<int32_t>(data, 0x1c4) > 0) {
            ++count;
        }
    }
    return count;
}

extern "C" LIBG_EXPORT
int32_t _ZN10LogicLevel15getPrototypeCountEv(void* self) {
    (void)self;
    int32_t count = 0;
    for (const auto& entry : g_objects_by_id) {
        void* obj = entry.second;
        if (!obj) {
            continue;
        }
        void* data = call_vmethod<void*>(obj, 0x58);
        auto* proto_list = load_at<int32_t*>(data, 0x1b8);
        if (proto_list && proto_list[2] > 0) {
            ++count;
        }
    }
    return count;
}

extern "C" LIBG_EXPORT
int32_t _ZN10LogicLevel16getBuildingCountEb(void* self, bool include_destroyed) {
    (void)self;
    int32_t count = 0;
    for (const auto& entry : g_objects_by_id) {
        void* obj = entry.second;
        if (!obj) {
            continue;
        }
        if (load_at<int32_t>(obj, 0) != 0) {
            continue;
        }
        if (!include_destroyed && load_at<int32_t>(obj, 0x18) <= 0) {
            continue;
        }
        ++count;
    }
    return count;
}

extern "C" LIBG_EXPORT
bool _ZNK10LogicLevel20isBuildingCapReachedEP17LogicBuildingDatab(void* self, void* data, bool notify) {
    if (!self || !data) {
        return false;
    }
    const bool is_artifact = load_at<int32_t>(data, 0x1e0) != 0;
    const bool is_prototype = load_at<int32_t>(data, 0x1e4) != 0;
    if (is_artifact) {
        const int32_t count = _ZN10LogicLevel14getArtifactCountEv(self);
        int32_t cap = 0;
        for (const auto& entry : g_objects_by_id) {
            void* obj = entry.second;
            if (!obj) {
                continue;
            }
            void* building_data = call_vmethod<void*>(obj, 0x58);
            if (building_data != data) {
                continue;
            }
            const int32_t upgrade = call_vmethod<int32_t>(obj, 0x10);
            const int32_t per_building = load_at<int32_t>(data, 0x1ec + upgrade * 4);
            if (per_building > cap) {
                cap = per_building;
            }
        }
        return count >= cap && cap > 0;
    }
    if (is_prototype) {
        const int32_t count = _ZN10LogicLevel15getPrototypeCountEv(self);
        int32_t cap = 0;
        for (const auto& entry : g_objects_by_id) {
            void* obj = entry.second;
            if (!obj) {
                continue;
            }
            void* building_data = call_vmethod<void*>(obj, 0x58);
            if (building_data != data) {
                continue;
            }
            const int32_t upgrade = call_vmethod<int32_t>(obj, 0x10);
            const auto* upgrades = load_at<int32_t*>(data, 0x1b8);
            if (upgrades && upgrade >= 0 && upgrades[2] > upgrade) {
                const int32_t per_building = upgrades[upgrade];
                if (per_building > cap) {
                    cap = per_building;
                }
            }
        }
        return count >= cap && cap > 0;
    }
    int32_t unlocked = 0;
    void* avatar = _ZNK10LogicLevel13getPlayerAvatarEv(self);
    if (avatar) {
        void* town_hall = libg_get_game_object_by_id(load_at<int32_t>(avatar, 0x78));
        if (town_hall) {
            const int32_t upgrade = call_vmethod<int32_t>(town_hall, 0x10);
            const int32_t town_hall_level = upgrade;
            (void)town_hall_level;
        }
    }
    unlocked = _ZN22LogicGameObjectManager24getGameObjectCountByDataEP8LogicData(nullptr, data);
    if (notify) {
        void* listener = load_at<void*>(self, 0x2c);
        if (listener) {
            call_vmethod<void>(listener, 0x2c, data);
        }
    }
    return unlocked > 0;
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
    if (!self) {
        return;
    }
    for (void* cmd : g_pending_commands) {
        destroy_known_command(cmd);
    }
    g_pending_commands.clear();
    _ZN19LogicCommandManager11setListenerEP27LogicCommandManagerListener(self, nullptr);
    *reinterpret_cast<void**>(static_cast<uint8_t*>(self) + 0x14) = nullptr;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManagerD1Ev(void* self) {
    _ZN18LogicCommandManagerD1Ev(self);
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager8destructEv(void* self) {
    _ZN18LogicCommandManagerD1Ev(self);
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManagerC1EP10LogicLevel(void* self, void* level) {
    _ZN18LogicCommandManagerC1Ev(self);
    if (self) {
        auto* listener = std::calloc(1, sizeof(void*) * 2);
        if (listener) {
            _ZN27LogicCommandManagerListenerC1Ev(listener);
        }
        *reinterpret_cast<void**>(static_cast<uint8_t*>(self) + 0x10) = listener;
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
    if (!self) {
        return;
    }
    void** slot = reinterpret_cast<void**>(static_cast<uint8_t*>(self) + 0x10);
    if (*slot) {
        call_vmethod<void>(*slot, 0x8);
        if (*slot) {
            call_vmethod<void>(*slot, 0x4);
        }
        *slot = nullptr;
    }
    *slot = listener;
}

extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(void*, void*, int32_t, int32_t, bool);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommandC1Eiii(void*, int32_t, int32_t, int32_t);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommandC1Eib(void*, int32_t, bool);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommandC1Ei(void*, int32_t);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommandC1Ei(void*, int32_t);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(void*, int32_t, void*);
extern "C" LIBG_EXPORT void _ZN31LogicSpeedUpConstructionCommandC1Ei(void*, int32_t);
extern "C" LIBG_EXPORT void _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(void*, void*);

namespace {
void* make_command(std::size_t size, int32_t type) {
    void* cmd = std::calloc(1, size);
    if (cmd) {
        std::memcpy(cmd, &type, sizeof(type));
        std::memset(static_cast<uint8_t*>(cmd) + 4, 0xff, sizeof(int32_t));
    }
    return cmd;
}
}

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager13createCommandEi(void* self, int32_t type) {
    (void)self;
    switch (type) {
    case 1: return make_command(0x1c, type);
    case 2: return make_command(0x10, type);
    case 3: return make_command(0x10, type);
    case 4: return make_command(0x14, type);
    case 5: return make_command(0x18, type);
    case 6: return make_command(0x14, type);
    case 7: return make_command(0x38, type);
    case 8: return make_command(0x38, type);
    case 9: return make_command(0x14, type);
    case 10: return make_command(0x10, type);
    case 11: return make_command(0x0c, type);
    case 12: return make_command(0x10, type);
    case 13: return make_command(0x0c, type);
    case 14: return make_command(0x10, type);
    case 15: return make_command(0x1c, type);
    case 16: return make_command(0x1c, type);
    case 17: return make_command(0x40, type);
    case 18: return make_command(0x18, type);
    case 19: return make_command(0x14, type);
    case 0x1ff:
    case 0x206:
    case 0x208:
    case 0x212:
    case 0x213:
    case 0x214:
    case 0x215:
    case 0x218:
    case 0x22a:
    case 0x22b:
    case 0x22c:
    case 0x22d:
    case 0x22e:
    case 0x22f:
    case 0x230:
    case 0x231:
    case 0x232:
    case 0x233:
    case 0x234:
    case 0x235:
    case 0x236:
    case 0x237:
    case 0x238:
    case 0x239:
    case 0x23a:
    case 0x23b:
    case 0x23c:
    case 0x23d:
    case 0x23e:
    case 0x23f:
    case 0x240:
    case 0x241:
    case 0x242:
    case 0x243:
    case 600:
    case 0x259:
        return nullptr;
    case 500: return make_command(0x20, type);
    case 0x1f5: return make_command(0x14, type);
    case 0x1f6: return make_command(0x10, type);
    case 0x1f7: return make_command(0x0c, type);
    case 0x1f8: return make_command(0x10, type);
    case 0x1fb: return make_command(0x18, type);
    case 0x1fc: return make_command(0x18, type);
    case 0x200: return make_command(0x14, type);
    case 0x201: return make_command(0x0c, type);
    case 0x202: return make_command(0x0c, type);
    case 0x203: return make_command(0x0c, type);
    case 0x204: return make_command(0x18, type);
    case 0x205: return make_command(0x10, type);
    case 0x207: return make_command(0x0c, type);
    case 0x209: return make_command(0x0c, type);
    case 0x20a: return make_command(0x10, type);
    case 0x20b: return make_command(0x0c, type);
    case 0x20c: return make_command(0x0c, type);
    case 0x20d: return make_command(0x08, type);
    case 0x20e: return make_command(0x0c, type);
    case 0x20f: return make_command(0x08, type);
    case 0x210: return make_command(0x10, type);
    case 0x211: return make_command(0x08, type);
    case 0x216: return make_command(0x10, type);
    case 0x217: return make_command(0x08, type);
    case 0x219: return make_command(0x0c, type);
    case 0x21a: return make_command(0x0c, type);
    case 0x21b: return make_command(0x0c, type);
    case 0x21c: return make_command(0x10, type);
    case 0x21d: return make_command(0x0c, type);
    case 0x21e: return make_command(0x0c, type);
    case 0x21f: return make_command(0x10, type);
    case 0x220: return make_command(0x10, type);
    case 0x221: return make_command(0x20, type);
    case 0x222: return make_command(0x20, type);
    case 0x223: return make_command(0x0c, type);
    case 0x224: return make_command(0x18, type);
    case 0x225: return make_command(0x20, type);
    case 0x226: return make_command(0x14, type);
    case 0x227: return make_command(0x0c, type);
    case 0x228: return make_command(0x10, type);
    case 0x229: return make_command(0x0c, type);
    case 0x244: return make_command(0x0c, type);
    case 0x245: return make_command(0x10, type);
    case 0x246: return make_command(0x08, type);
    case 0x247: return make_command(0x08, type);
    case 0x248: return make_command(0x0c, type);
    case 0x249: return make_command(0x0c, type);
    case 0x24a: return make_command(0x10, type);
    case 0x24b: return make_command(0x10, type);
    case 0x24c: return make_command(0x20, type);
    case 0x24d: return make_command(0x10, type);
    case 0x24e: return make_command(0x08, type);
    case 0x24f: return make_command(0x18, type);
    case 0x250: return make_command(0x0c, type);
    case 0x251: return make_command(0x0c, type);
    case 0x252: return make_command(0x0c, type);
    case 0x253: return make_command(0x0c, type);
    case 0x254: return make_command(0x0c, type);
    case 0x255: return make_command(0x0c, type);
    case 0x256: return make_command(0x0c, type);
    case 0x25a: return make_command(0x08, type);
    case 0x25b: return make_command(0x08, type);
    case 0x25c: return make_command(0x14, type);
    case 0x25d: return make_command(0x18, type);
    case 0x25e: return make_command(0x08, type);
    case 700: return make_command(0x0c, type);
    default:
        break;
    }
    return nullptr;
}

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager13decodeCommandEP10ByteStreambb(void* self, void* stream, bool a, bool b) {
    (void)a;
    (void)b;
    if (!stream) {
        return nullptr;
    }
    const int32_t type = read_u32(stream);
    void* cmd = _ZN19LogicCommandManager13createCommandEi(self, type);
    if (!cmd) {
        return nullptr;
    }
    switch (type) {
    case 500: _ZN23LogicBuyBuildingCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x1f5: _ZN24LogicMoveBuildingCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x1f6: _ZN27LogicUpgradeBuildingCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x1f7: _ZN24LogicSellBuildingCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x1f8: _ZN31LogicSpeedUpConstructionCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x1fb: _ZN25LogicClearObstacleCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x1fc: _ZN21LogicTrainUnitCommand6decodeEP10ByteStream(cmd, stream); break;
    case 0x24f: _ZN24LogicBuyResourcesCommand6decodeEP10ByteStream(cmd, stream); break;
    default: break;
    }
    return cmd;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    for (void* cmd : g_pending_commands) {
        destroy_known_command(cmd);
    }
    g_pending_commands.clear();
    const uint32_t count = read_u32(stream);
    for (uint32_t i = 0; i < count; ++i) {
        void* cmd = _ZN19LogicCommandManager13decodeCommandEP10ByteStreambb(self, stream, false, false);
        if (cmd) {
            g_pending_commands.push_back(cmd);
        }
    }
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager13encodeCommandEP15ChecksumEncoderP12LogicCommand(void* self, void* encoder, void* cmd) {
    (void)self;
    if (!encoder || !cmd) {
        return;
    }
    switch (command_type(cmd)) {
    case 500: _ZN23LogicBuyBuildingCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x1f5: _ZN24LogicMoveBuildingCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x1f6: _ZN27LogicUpgradeBuildingCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x1f7: _ZN24LogicSellBuildingCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x1f8: _ZN31LogicSpeedUpConstructionCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x1fb: _ZN25LogicClearObstacleCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x1fc: _ZN21LogicTrainUnitCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    case 0x24f: _ZN24LogicBuyResourcesCommand6encodeEP15ChecksumEncoder(cmd, encoder); break;
    default: break;
    }
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager17saveCommandToJSONEP15LogicJSONObjectP12LogicCommand(void* self, void* json, void* cmd) {
    (void)self;
    if (!json || !cmd) {
        return;
    }
    std::memcpy(static_cast<uint8_t*>(json) + 0, cmd, sizeof(int32_t));
    const int32_t sub_tick = *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(cmd) + 4);
    std::memcpy(static_cast<uint8_t*>(json) + 4, &sub_tick, sizeof(sub_tick));
}

extern "C" LIBG_EXPORT
void* _ZN19LogicCommandManager19loadCommandFromJSONEP15LogicJSONObject(void* self, void* json) {
    if (!json) {
        return nullptr;
    }
    const int32_t type = *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(json) + 0);
    void* cmd = _ZN19LogicCommandManager13createCommandEi(self, type);
    if (!cmd) {
        return nullptr;
    }
    const int32_t sub_tick = *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(json) + 4);
    std::memcpy(static_cast<uint8_t*>(cmd) + 4, &sub_tick, sizeof(sub_tick));
    return cmd;
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, static_cast<uint32_t>(g_pending_commands.size()));
    for (void* cmd : g_pending_commands) {
        write_u32(encoder, static_cast<uint32_t>(command_type(cmd)));
        _ZN19LogicCommandManager13encodeCommandEP15ChecksumEncoderP12LogicCommand(self, encoder, cmd);
    }
}

extern "C" LIBG_EXPORT
void _ZN19LogicCommandManager7subTickEv(void* self) {
    if (!self) {
        return;
    }
    void* level = *reinterpret_cast<void**>(static_cast<uint8_t*>(self) + 0x14);
    if (!level) {
        return;
    }
    const int32_t state = *reinterpret_cast<int32_t*>(level);
    const int32_t tick = *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(level) + 0x48);
    for (std::size_t i = 0; i < g_pending_commands.size();) {
        void* cmd = g_pending_commands[i];
        const int32_t exec_tick = cmd ? *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(cmd) + 4) : 0;
        if (state == 5 && exec_tick <= tick) {
            dispatch_command(cmd, level);
            destroy_known_command(cmd);
            g_pending_commands.erase(g_pending_commands.begin() + static_cast<long>(i));
            continue;
        }
        if (state != 5 && exec_tick == 0) {
            *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(cmd) + 4) = tick;
        }
        ++i;
    }
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
    const int32_t state = *reinterpret_cast<const int32_t*>(level);
    const bool stage_one = (type >= 500 && type <= 599) || (type > 0x243 && type < 600) || state == 1;
    const bool stage_two = (type >= 0x244 && type <= 599) || state == 4 || state == 1;
    const bool stage_three = (type >= 600 && type <= 699) || state == 2 || state == 5;
    if (type >= 1000 || !(stage_one && stage_two && stage_three)) {
        std::abort();
    }
}

extern "C" LIBG_EXPORT
void _ZN27LogicCommandManagerListenerC1Ev(void* self) {
    if (self) {
        static LogicCommandManagerListenerVTable vtable{
            [](void* obj) { (void)obj; },
            [](void* obj) { std::free(obj); },
        };
        *reinterpret_cast<LogicCommandManagerListenerVTable**>(self) = &vtable;
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
    destroy_known_command(cmd);
}

extern "C" LIBG_EXPORT
bool _ZN18LogicCommandManager11executeCommandEPN5Logic7CommandE(void* self, void* cmd) {
    if (!self || !cmd) {
        return false;
    }
    libg_register_command(cmd);
    return true;
}

extern "C" LIBG_EXPORT
void _ZN18LogicCommandManager8undoLastEv(void* self) {
    (void)self;
    if (!g_pending_commands.empty()) {
        destroy_known_command(g_pending_commands.back());
        g_pending_commands.pop_back();
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
