#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ============================================================================
// Game command implementations.
// The original binary has real logic here; these bodies are ported from the
// decompiled analysis and kept conservative where the surrounding ABI surface
// is still incomplete.
// ============================================================================

extern "C" LIBG_EXPORT void* libg_get_game_object_by_id(int32_t id);
extern "C" LIBG_EXPORT void libg_register_command(void* cmd);
extern "C" LIBG_EXPORT int32_t libg_get_wallet_amount(int32_t resource_type);
extern "C" LIBG_EXPORT bool libg_spend_wallet_amount(int32_t resource_type, int32_t amount);
extern "C" LIBG_EXPORT void libg_add_wallet_amount(int32_t resource_type, int32_t amount);
extern "C" LIBG_EXPORT void libg_add_game_object(void* obj);
extern "C" LIBG_EXPORT void libg_remove_game_object(void* obj);
extern "C" LIBG_EXPORT void* libg_get_game_object_by_id(int32_t id);
extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void* _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void* _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(void* self, void* level);

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

inline void* level_object_manager(void* level) {
    return load_at<void*>(level, 0x4c);
}

inline void* make_opaque_block(std::size_t size) {
    return std::calloc(1, size);
}
} // namespace

extern "C" LIBG_EXPORT
void _ZN21LogicBuyBuildingCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(self, level);
    }
}

extern "C" LIBG_EXPORT
void _ZN20LogicTrainUnitCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(self, level);
    }
}

extern "C" LIBG_EXPORT
void _ZN21LogicMoveBuildingCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(self, level);
    }
}

extern "C" LIBG_EXPORT
void _ZN25LogicUpgradeBuildingCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(self, level);
    }
}

extern "C" LIBG_EXPORT
void _ZN21LogicSellBuildingCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (!level) {
        return;
    }
    auto* obj_mgr = level_object_manager(level);
    const int32_t building_id = load_at<int32_t>(self, 8);
    void* building = libg_get_game_object_by_id(building_id);
    if (building && obj_mgr) {
        libg_remove_game_object(building);
    }
}

extern "C" LIBG_EXPORT
void _ZN24LogicClearObstacleCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(self, level);
    }
}

extern "C" LIBG_EXPORT
void _ZN21LogicBuyResourcesCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(self, level);
    }
}

extern "C" LIBG_EXPORT
void _ZN28LogicSpeedUpConstructionCommand7executeEv(void* self) {
    const auto level = load_at<void*>(self, 0x18);
    if (level) {
        _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(self, level);
    }
}

// ============================================================================
// Command validation
// ============================================================================

extern "C" LIBG_EXPORT
bool _ZN21LogicBuyBuildingCommand6canExecuteEPN5Logic5StateE(void* self, void* state) {
    if (!self || !state) {
        return false;
    }

    return true;
}

// ============================================================================
// Command factory
// ============================================================================

extern "C" LIBG_EXPORT
void* _ZN20LogicCommandFactory9createCommandEN5Logic12CommandTypeE(int32_t type) {
    (void)type;
    return nullptr;
}

// ============================================================================
// Source-backed class-method exports recovered from the original binary.
// These mirror the decompiled originals: mostly empty bodies and constant
// getters for command type metadata.
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(void* self, void* data, int32_t a, int32_t b, bool c) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x20);
    store_at<int32_t>(self, 0, 500);
    store_at<void*>(self, 8, data);
    store_at<int32_t>(self, 0xc, a);
    store_at<int32_t>(self, 0x10, b);
    store_at<uint8_t>(self, 0x14, static_cast<uint8_t>(c));
    store_at<int32_t>(self, 0x18, -1);
}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommandC2EP17LogicBuildingDataiib(void* self, void* data, int32_t a, int32_t b, bool c) {
    _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(self, data, a, b, c);
}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level || !load_at<void*>(self, 8)) {
        return;
    }

    auto* obj_mgr = level_object_manager(level);
    if (!obj_mgr) {
        return;
    }

    void* building = make_opaque_block(0x100);
    if (!building) {
        return;
    }

    store_at<int32_t>(building, 0, 0x100);
    store_at<int32_t>(building, 0x10, load_at<int32_t>(self, 0x0c) << 9);
    store_at<int32_t>(building, 0x14, load_at<int32_t>(self, 0x10) << 9);
    store_at<int32_t>(building, 0x18, load_at<int32_t>(self, 0x18));
    libg_add_game_object(building);
}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand12executeBuildEP10LogicLevelP13LogicBuildingS3_S3_(void* self, void* level, void* p1, void* p2, void* p3) {
    if (!self || !level) {
        return;
    }

    auto* obj_mgr = level_object_manager(level);
    void* building = make_opaque_block(0x100);
    if (!building) {
        return;
    }

    store_at<int32_t>(building, 0, 0x100);
    store_at<int32_t>(building, 0x10, load_at<int32_t>(self, 0x0c) << 9);
    store_at<int32_t>(building, 0x14, load_at<int32_t>(self, 0x10) << 9);
    store_at<void*>(building, 0x20, p1);
    store_at<void*>(building, 0x24, p2);
    store_at<void*>(building, 0x28, p3);
    if (obj_mgr) {
        libg_add_game_object(building);
    }
}

extern "C" LIBG_EXPORT
int32_t _ZNK23LogicBuyBuildingCommand14getCommandTypeEv(void* self) { return 500; }

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void* _ZN23LogicBuyBuildingCommand15getResourceCostEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void* _ZNK23LogicBuyBuildingCommand13getTargetDataEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(void* self, int32_t a, void* data) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x18);
    store_at<int32_t>(self, 0, 0x1fc);
    store_at<int32_t>(self, 8, a);
    store_at<void*>(self, 0xc, data);
    store_at<int32_t>(self, 0x10, data ? 1 : 0);
}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommandC2EiP19LogicCombatItemData(void* self, int32_t a, void* data) {
    _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(self, a, data);
}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }
    auto* obj_mgr = level_object_manager(level);
    if (!obj_mgr) {
        return;
    }

    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (!building) {
        return;
    }

    const int32_t training_amount = load_at<int32_t>(self, 0x10) > 0 ? load_at<int32_t>(self, 0x10) : 1;
    libg_spend_wallet_amount(static_cast<int32_t>(ResourceType::Gold), training_amount);
    store_at<int32_t>(building, 0x30, load_at<int32_t>(building, 0x30) + training_amount);
}

extern "C" LIBG_EXPORT
int32_t _ZNK21LogicTrainUnitCommand14getCommandTypeEv(void* self) { return 0x1fc; }

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void* _ZN21LogicTrainUnitCommand15getResourceCostEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void* _ZNK21LogicTrainUnitCommand13getTargetDataEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommandC1Eiii(void* self, int32_t a, int32_t b, int32_t c) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x14);
    store_at<int32_t>(self, 0, 0x1f5);
    store_at<int32_t>(self, 8, a);
    store_at<int32_t>(self, 0xc, b);
    store_at<int32_t>(self, 0x10, c);
}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommandC2Eiii(void* self, int32_t a, int32_t b, int32_t c) {
    _ZN24LogicMoveBuildingCommandC1Eiii(self, a, b, c);
}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }

    auto* obj_mgr = level_object_manager(level);
    if (!obj_mgr) {
        return;
    }

    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (!building) {
        return;
    }

    store_at<int32_t>(building, 0x10, load_at<int32_t>(self, 0xc) << 9);
    store_at<int32_t>(building, 0x14, load_at<int32_t>(self, 0x10) << 9);
}

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicMoveBuildingCommand14getCommandTypeEv(void* self) { return 0x1f5; }

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommandC1Ei(void* self, int32_t a) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x10);
    store_at<int32_t>(self, 0, 0x1f7);
    store_at<int32_t>(self, 8, a);
}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommandC2Ei(void* self, int32_t a) {
    _ZN24LogicSellBuildingCommandC1Ei(self, a);
}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }
    auto* obj_mgr = level_object_manager(level);
    if (!obj_mgr) {
        return;
    }
    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (building) {
        libg_remove_game_object(building);
    }
}

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicSellBuildingCommand14getCommandTypeEv(void* self) { return 0x1f7; }

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommandC1Ei(void* self, int32_t a) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x18);
    store_at<int32_t>(self, 0, 0x1fb);
    store_at<int32_t>(self, 0x14, -1);

    auto* list = static_cast<uintptr_t*>(std::calloc(3, sizeof(uintptr_t)));
    auto* ids = static_cast<int32_t*>(std::calloc(1, sizeof(int32_t)));
    if (list && ids) {
        list[0] = reinterpret_cast<uintptr_t>(ids);
        list[1] = 1;
        list[2] = 1;
        ids[0] = a;
    }
    store_at<void*>(self, 8, list);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommandC2Ei(void* self, int32_t a) {
    _ZN25LogicClearObstacleCommandC1Ei(self, a);
}

extern "C" LIBG_EXPORT
int32_t _ZN25LogicClearObstacleCommand17getObstacleGlobalIDEv(void* self) {
    if (!self) {
        return 0;
    }
    auto* list = load_at<uintptr_t*>(self, 8);
    if (!list || list[2] != 1 || !list[0]) {
        return 0;
    }
    return reinterpret_cast<int32_t*>(list[0])[0];
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }

    auto* obj_mgr = level_object_manager(level);
    auto* list = load_at<int32_t*>(self, 8);
    if (!obj_mgr || !list) {
        return;
    }

    const int32_t count = list[2];
    const auto* ids = reinterpret_cast<int32_t*>(list[0]);
    for (int32_t i = 0; i < count; ++i) {
        void* obstacle = libg_get_game_object_by_id(ids ? ids[i] : 0);
        if (obstacle) {
            store_at<int32_t>(obstacle, 0x18, 1);
        }
    }

    if (load_at<int32_t>(self, 0x14) >= 0) {
        store_at<int32_t>(level, 0x20, load_at<int32_t>(self, 0x14));
    }
}

extern "C" LIBG_EXPORT
int32_t _ZNK25LogicClearObstacleCommand14getCommandTypeEv(void* self) { return 0x1fb; }

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand20setObstacleGlobalIdsEP14LogicArrayListIiE(void* self, void* ids) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand15getResourceCostEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand24getOneResourceCostAmountEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand22getOneResourceCostDataEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void* _ZNK25LogicClearObstacleCommand13getTargetDataEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommandC1Eib(void* self, int32_t a, bool b) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x10);
    store_at<int32_t>(self, 0, 0x1f6);
    store_at<int32_t>(self, 8, a);
    store_at<uint8_t>(self, 0xc, static_cast<uint8_t>(b));
}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommandC2Eib(void* self, int32_t a, bool b) {
    _ZN27LogicUpgradeBuildingCommandC1Eib(self, a, b);
}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }
    auto* obj_mgr = level_object_manager(level);
    if (!obj_mgr) {
        return;
    }

    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (!building) {
        return;
    }

    if (load_at<uint8_t>(self, 0xc)) {
        libg_spend_wallet_amount(static_cast<int32_t>(ResourceType::Diamonds), 1);
        store_at<int32_t>(building, 0x28, 1);
    } else {
        libg_spend_wallet_amount(static_cast<int32_t>(ResourceType::Gold), 1);
        store_at<int32_t>(building, 0x28, 0);
    }
}

extern "C" LIBG_EXPORT
int32_t _ZNK27LogicUpgradeBuildingCommand14getCommandTypeEv(void* self) { return 0x1f6; }

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void* _ZN27LogicUpgradeBuildingCommand15getResourceCostEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void* _ZNK27LogicUpgradeBuildingCommand13getTargetDataEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommandC1Ei(void* self, int32_t a) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x10);
    store_at<int32_t>(self, 0, 0x1f8);
    store_at<int32_t>(self, 8, a);
}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommandC2Ei(void* self, int32_t a) {
    _ZN31LogicSpeedUpConstructionCommandC1Ei(self, a);
}

extern "C" LIBG_EXPORT
void* _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return nullptr;
    }
    auto* obj_mgr = level_object_manager(level);
    if (!obj_mgr) {
        return nullptr;
    }
    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (building) {
        store_at<int32_t>(building, 0x2c, 1);
    }
    return building;
}

extern "C" LIBG_EXPORT
int32_t _ZNK31LogicSpeedUpConstructionCommand14getCommandTypeEv(void* self) { return 0x1f8; }

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand11getResourceEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void* _ZN24LogicBuyResourcesCommand16getResourceCountEP10LogicLevel(void* self, void* level) {
    (void)level;
    if (!self) {
        return nullptr;
    }
    const bool custom_resource = load_at<void*>(self, 8) != nullptr;
    const int32_t amount = custom_resource
        ? libg_get_wallet_amount(static_cast<int32_t>(ResourceType::Wood))
        : libg_get_wallet_amount(static_cast<int32_t>(ResourceType::Gold));
    return reinterpret_cast<void*>(static_cast<intptr_t>(amount));
}

extern "C" LIBG_EXPORT
void* _ZN24LogicBuyResourcesCommand19getMaxResourceCountEP10LogicLevel(void* self, void* level) {
    (void)level;
    if (!self) {
        return nullptr;
    }
    return reinterpret_cast<void*>(static_cast<intptr_t>(10000));
}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(void* self, void* base) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x18);
    store_at<int32_t>(self, 0, 0x24f);
    store_at<void*>(self, 8, base);
    store_at<uint8_t>(self, 0xc, 0);
}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommandC2EP27LogicResourceCostingCommand(void* self, void* base) {
    _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(self, base);
}

extern "C" LIBG_EXPORT
void* _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return nullptr;
    }
    if (!libg_spend_wallet_amount(static_cast<int32_t>(ResourceType::Diamonds), 1)) {
        return nullptr;
    }
    libg_add_wallet_amount(static_cast<int32_t>(ResourceType::Gold), 1);
    return self;
}

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicBuyResourcesCommand14getCommandTypeEv(void* self) { return 0x24f; }

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand6decodeEP10ByteStream(void* self, void* stream) {}

// Base command classes recovered from the decompiled source.
extern "C" LIBG_EXPORT
void _ZN12LogicCommandC1Ev(void* self) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommandC2Ev(void* self) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZNK12LogicCommand22isResourceCostingCommandEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZNK12LogicCommand17getJSONForReplayEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand13loadFromJSONEP15LogicJSONObject(void* self, void* json) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZNK12LogicCommand14getCommandTypeEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand18setExecuteSubTickEi(void* self, int32_t sub_tick) {
    (void)self;
    (void)sub_tick;
}

extern "C" LIBG_EXPORT
int32_t _ZNK12LogicCommand17getExecuteSubTickEv(void* self) {
    int32_t out = 0;
    if (self) {
        std::memcpy(&out, static_cast<uint8_t*>(self) + 4, sizeof(out));
    }
    return out;
}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommandC1Ev(void* self) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommandC2Ev(void* self) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZNK27LogicResourceCostingCommand22isResourceCostingCommandEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand24getOneResourceCostAmountEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand22getOneResourceCostDataEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZNK27LogicResourceCostingCommand13getTargetDataEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand15getResourceCostEP10LogicLevel(void* self, void* level) {}
