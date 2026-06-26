#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>

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
extern "C" LIBG_EXPORT void* _ZN19LogicCommandManager13createCommandEi(void* self, int32_t type);
extern "C" LIBG_EXPORT void _ZN19LogicCommandManager13encodeCommandEP15ChecksumEncoderP12LogicCommand(void* self, void* encoder, void* cmd);
extern "C" LIBG_EXPORT void* _ZN19LogicCommandManager13decodeCommandEP10ByteStreambb(void* self, void* stream, bool a, bool b);
extern "C" LIBG_EXPORT void _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN24LogicSellBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void* _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void* _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT void _ZN12LogicCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN12LogicCommandC1Ev(void* self);
extern "C" LIBG_EXPORT void _ZN12LogicCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN12LogicCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder);
extern "C" LIBG_EXPORT void _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(void* self, void* stream);
extern "C" LIBG_EXPORT void _ZN27LogicResourceCostingCommand8destructEv(void* self);
extern "C" LIBG_EXPORT void _ZN27LogicResourceCostingCommandC1Ev(void* self);
extern "C" LIBG_EXPORT int32_t _ZN24LogicBuyResourcesCommand16getResourceCountEP10LogicLevel(void* self, void* level);
extern "C" LIBG_EXPORT int32_t _ZN24LogicBuyResourcesCommand19getMaxResourceCountEP10LogicLevel(void* self, void* level);

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

inline void* level_player_avatar(void* level) {
    if (!level) {
        return nullptr;
    }
    void* mode = load_at<void*>(level, 0x40);
    const int32_t state = load_at<int32_t>(mode, 4);
    const std::size_t offset = (state == 1 || state == 3) ? 0x38 : 0x3c;
    return load_at<void*>(level, offset);
}

inline void* make_opaque_block(std::size_t size) {
    return std::calloc(1, size);
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

inline void write_flag(void* encoder, int32_t value) {
    if (encoder) {
        call_vmethod<void>(encoder, 0x18, value);
    }
}

inline uint32_t read_u32(void* stream) {
    return stream ? call_vmethod<uint32_t>(stream, 0x54) : 0;
}

inline int32_t read_flag(void* stream) {
    return stream ? call_vmethod<int32_t>(stream, 0x50) : 0;
}

inline int32_t command_type(void* cmd) {
    return cmd ? load_at<int32_t>(cmd, 0) : 0;
}

inline void execute_known_command(void* cmd, void* level) {
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

inline void clear_obstacle_list(void* self) {
    auto* list = load_at<uintptr_t*>(self, 8);
    if (!list) {
        return;
    }
    if (list[0]) {
        std::free(reinterpret_cast<void*>(list[0]));
    }
    std::free(list);
    store_at<void*>(self, 8, nullptr);
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
    return _ZN19LogicCommandManager13createCommandEi(nullptr, type);
}

// ============================================================================
// Source-backed class-method exports recovered from the original binary.
// These mirror the decompiled originals: mostly empty bodies and constant
// getters for command type metadata.
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    _ZN27LogicResourceCostingCommand8destructEv(self);
    store_at<void*>(self, 8, nullptr);
    store_at<int32_t>(self, 0xc, 0);
    store_at<int32_t>(self, 0x10, 0);
    store_at<uint8_t>(self, 0x14, 0);
    store_at<int32_t>(self, 0x18, -1);
}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(void* self, void* data, int32_t a, int32_t b, bool c) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x20);
    store_at<int32_t>(self, 0, 500);
    store_at<int32_t>(self, 4, -1);
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
void _ZN23LogicBuyBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 0x0c));
    write_u32(encoder, load_at<uint32_t>(self, 0x10));
    write_u32(encoder, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(load_at<void*>(self, 8))));
    write_flag(encoder, load_at<uint8_t>(self, 0x14));
    write_u32(encoder, load_at<uint32_t>(self, 0x18));
    _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<uint32_t>(self, 0x0c, read_u32(stream));
    store_at<uint32_t>(self, 0x10, read_u32(stream));
    store_at<void*>(self, 8, reinterpret_cast<void*>(static_cast<uintptr_t>(read_u32(stream))));
    store_at<uint8_t>(self, 0x14, static_cast<uint8_t>(read_flag(stream)));
    store_at<uint32_t>(self, 0x18, read_u32(stream));
    _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void* _ZN23LogicBuyBuildingCommand15getResourceCostEP10LogicLevel(void* self, void* level) {
    void* data = load_at<void*>(self, 8);
    if (!self || !level || !data) {
        return nullptr;
    }
    if (load_at<uint8_t>(level, 0xc9) != 0) {
        return nullptr;
    }
    if (call_vmethod<int32_t>(data, 0x60) != 0) {
        return nullptr;
    }
    return call_vmethod<void*>(data, 0x58);
}

extern "C" LIBG_EXPORT
void* _ZNK23LogicBuyBuildingCommand13getTargetDataEP10LogicLevel(void* self, void* level) {
    (void)self;
    return level ? load_at<void*>(level, 8) : nullptr;
}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    if (load_at<void*>(self, 0x14)) {
        std::free(load_at<void*>(self, 0x14));
        store_at<void*>(self, 0x14, nullptr);
    }
    _ZN27LogicResourceCostingCommand8destructEv(self);
    store_at<int32_t>(self, 8, 0);
    store_at<void*>(self, 0xc, nullptr);
    store_at<int32_t>(self, 0x10, 0);
    store_at<void*>(self, 0x14, nullptr);
}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(void* self, int32_t a, void* data) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x18);
    store_at<int32_t>(self, 0, 0x1fc);
    store_at<int32_t>(self, 4, -1);
    store_at<int32_t>(self, 8, a);
    store_at<void*>(self, 0xc, data);
    store_at<int32_t>(self, 0x10, data ? call_vmethod<int32_t>(data, 0x24) : 0);
    store_at<void*>(self, 0x14, nullptr);
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
void _ZN21LogicTrainUnitCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 8));
    write_u32(encoder, load_at<uint32_t>(self, 0x10));
    write_u32(encoder, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(load_at<void*>(self, 0xc))));
    _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<uint32_t>(self, 8, read_u32(stream));
    const int32_t mode = static_cast<int32_t>(read_u32(stream));
    store_at<int32_t>(self, 0x10, mode);
    store_at<void*>(self, 0xc, reinterpret_cast<void*>(static_cast<uintptr_t>(read_u32(stream))));
    _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void* _ZN21LogicTrainUnitCommand15getResourceCostEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return nullptr;
    }
    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (!building || call_vmethod<int32_t>(building, 0x60) != 0) {
        return nullptr;
    }
    if (!load_at<void*>(self, 0x14)) {
        store_at<void*>(self, 0x14, make_opaque_block(0x30));
    }
    return load_at<void*>(self, 0x14);
}

extern "C" LIBG_EXPORT
void* _ZNK21LogicTrainUnitCommand13getTargetDataEP10LogicLevel(void* self, void* level) {
    (void)self;
    return level ? load_at<void*>(level, 0xc) : nullptr;
}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    _ZN12LogicCommand8destructEv(self);
    store_at<int32_t>(self, 8, 0);
    store_at<int32_t>(self, 0xc, 0);
    store_at<int32_t>(self, 0x10, 0);
}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommandC1Eiii(void* self, int32_t a, int32_t b, int32_t c) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x14);
    store_at<int32_t>(self, 0, 0x1f5);
    store_at<int32_t>(self, 4, -1);
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
void _ZN24LogicMoveBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 0x0c));
    write_u32(encoder, load_at<uint32_t>(self, 0x10));
    write_u32(encoder, load_at<uint32_t>(self, 8));
    _ZN12LogicCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<uint32_t>(self, 0x0c, read_u32(stream));
    store_at<uint32_t>(self, 0x10, read_u32(stream));
    store_at<uint32_t>(self, 8, read_u32(stream));
    _ZN12LogicCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    _ZN12LogicCommand8destructEv(self);
    store_at<int32_t>(self, 8, 0);
}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommandC1Ei(void* self, int32_t a) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x10);
    store_at<int32_t>(self, 0, 0x1f7);
    store_at<int32_t>(self, 4, -1);
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
void _ZN24LogicSellBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 8));
    _ZN12LogicCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<int32_t>(self, 8, static_cast<int32_t>(read_u32(stream)));
    _ZN12LogicCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    clear_obstacle_list(self);
    if (load_at<void*>(self, 0x0c)) {
        std::free(load_at<void*>(self, 0x0c));
    }
    _ZN27LogicResourceCostingCommand8destructEv(self);
    store_at<void*>(self, 8, nullptr);
    store_at<void*>(self, 0xc, nullptr);
    store_at<uint8_t>(self, 0x10, 0);
    store_at<int32_t>(self, 0x14, -1);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommandC1Ei(void* self, int32_t a) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x18);
    store_at<int32_t>(self, 0, 0x1fb);
    store_at<int32_t>(self, 4, -1);
    store_at<void*>(self, 0xc, nullptr);
    store_at<uint8_t>(self, 0x10, 0);
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
    return load_at<int32_t>(reinterpret_cast<void*>(list[0]), 0);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand11setObstaclesEP14LogicArrayListIP15LogicGameObjectE(void* self, void* obstacles) {
    if (!self) {
        return;
    }
    clear_obstacle_list(self);
    if (!obstacles) {
        store_at<void*>(self, 8, nullptr);
        return;
    }
    const int32_t count = load_at<int32_t>(obstacles, 8);
    if (count <= 0) {
        store_at<void*>(self, 8, nullptr);
        return;
    }
    auto* list = static_cast<uintptr_t*>(std::calloc(3, sizeof(uintptr_t)));
    auto* ids = static_cast<int32_t*>(std::calloc(static_cast<std::size_t>(count), sizeof(int32_t)));
    if (!list || !ids) {
        std::free(list);
        std::free(ids);
        store_at<void*>(self, 8, nullptr);
        return;
    }
    list[0] = reinterpret_cast<uintptr_t>(ids);
    list[1] = static_cast<uintptr_t>(count);
    list[2] = static_cast<uintptr_t>(count);
    const auto* input_array = load_at<uintptr_t*>(obstacles, 0);
    if (count > 0 && input_array) {
        ids[0] = load_at<int32_t>(reinterpret_cast<void*>(input_array[0]), 0);
        for (int32_t i = 1; i < count; ++i) {
            ids[i] = load_at<int32_t>(reinterpret_cast<void*>(input_array[i]), 0);
        }
    }
    store_at<void*>(self, 8, list);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand15loadLayoutEi(void* self, int32_t layout_id) {
    if (self) {
        store_at<int32_t>(self, 0x14, layout_id);
    }
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
void _ZN25LogicClearObstacleCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(self, encoder);
    auto* ids = load_at<uintptr_t*>(self, 8);
    if (!ids) {
        write_u32(encoder, 0);
    } else {
        const auto* values = reinterpret_cast<int32_t*>(ids[0]);
        const int32_t count = static_cast<int32_t>(ids[2]);
        write_u32(encoder, values ? static_cast<uint32_t>(values[0]) : 0);
        for (int32_t i = 1; i < count; ++i) {
            write_u32(encoder, static_cast<uint32_t>(values[i]));
        }
    }
    write_flag(encoder, load_at<uint8_t>(self, 0x10));
    write_u32(encoder, load_at<uint32_t>(self, 0x14));
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    const int32_t count = static_cast<int32_t>(read_u32(stream));
    if (count > 0) {
        auto* list = static_cast<uintptr_t*>(std::calloc(3, sizeof(uintptr_t)));
        auto* ids = static_cast<int32_t*>(std::calloc(static_cast<std::size_t>(count), sizeof(int32_t)));
        if (list && ids) {
            list[0] = reinterpret_cast<uintptr_t>(ids);
            list[1] = count;
            list[2] = count;
            ids[0] = static_cast<int32_t>(read_u32(stream));
            for (int32_t i = 1; i < count; ++i) {
                ids[i] = static_cast<int32_t>(read_u32(stream));
            }
        }
        store_at<void*>(self, 8, list);
    } else {
        store_at<void*>(self, 8, nullptr);
    }
    store_at<uint8_t>(self, 0x10, static_cast<uint8_t>(read_flag(stream)));
    store_at<uint32_t>(self, 0x14, read_u32(stream));
    _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand20setObstacleGlobalIdsEP14LogicArrayListIiE(void* self, void* ids) {
    if (!self) {
        return;
    }
    clear_obstacle_list(self);
    if (load_at<void*>(self, 0x0c)) {
        std::free(load_at<void*>(self, 0x0c));
        store_at<void*>(self, 0x0c, nullptr);
    }
    store_at<void*>(self, 8, ids);
}

extern "C" LIBG_EXPORT
void* _ZN25LogicClearObstacleCommand15getResourceCostEP10LogicLevel(void* self, void* level) {
    if (!self || !level || !load_at<void*>(self, 8)) {
        return nullptr;
    }
    auto* list = load_at<uintptr_t*>(self, 8);
    const int32_t count = list ? static_cast<int32_t>(list[2]) : 0;
    if (count <= 0) {
        return nullptr;
    }
    if (!load_at<void*>(self, 0x0c)) {
        store_at<void*>(self, 0x0c, make_opaque_block(0x30));
    }
    void* avatar = level_player_avatar(level);
    const int32_t avatar_level = avatar ? call_vmethod<int32_t>(avatar, 0xf4) : 0;
    const auto* ids = reinterpret_cast<int32_t*>(list[0]);
    for (int32_t i = 0; i < count; ++i) {
        void* obstacle = libg_get_game_object_by_id(ids ? ids[i] : 0);
        if (!obstacle) {
            return nullptr;
        }
        if (load_at<int32_t>(obstacle, 0xb4) != 0 || load_at<int32_t>(obstacle, 0xb8) != 0) {
            return nullptr;
        }
        if (load_at<int32_t>(obstacle, 0x18) != 3) {
            return nullptr;
        }
        void* data = call_vmethod<void*>(obstacle, 0x58);
        if (!data || avatar_level < load_at<int32_t>(data, 0x128)) {
            return nullptr;
        }
    }
    return load_at<void*>(self, 0x0c);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand24getOneResourceCostAmountEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }
    (void)_ZN25LogicClearObstacleCommand15getResourceCostEP10LogicLevel(self, level);
}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand22getOneResourceCostDataEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }
    (void)_ZN25LogicClearObstacleCommand15getResourceCostEP10LogicLevel(self, level);
}

extern "C" LIBG_EXPORT
void* _ZNK25LogicClearObstacleCommand13getTargetDataEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return nullptr;
    }
    const auto* ids = load_at<uintptr_t*>(self, 8);
    if (!ids || ids[2] != 1 || !ids[0]) {
        return nullptr;
    }
    void* obstacle = libg_get_game_object_by_id(static_cast<int32_t>(ids[0]));
    if (!obstacle) {
        return nullptr;
    }
    if (load_at<int32_t>(obstacle, 0x18) != 3) {
        return nullptr;
    }
    return call_vmethod<void*>(obstacle, 0x58);
}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    _ZN27LogicResourceCostingCommand8destructEv(self);
    store_at<int32_t>(self, 8, 0);
    store_at<uint8_t>(self, 0xc, 0);
}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommandC1Eib(void* self, int32_t a, bool b) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x10);
    store_at<int32_t>(self, 0, 0x1f6);
    store_at<int32_t>(self, 4, -1);
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
void _ZN27LogicUpgradeBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 8));
    write_flag(encoder, load_at<uint8_t>(self, 0xc));
    _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<uint32_t>(self, 8, read_u32(stream));
    store_at<uint8_t>(self, 0xc, static_cast<uint8_t>(read_flag(stream)));
    _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void* _ZN27LogicUpgradeBuildingCommand15getResourceCostEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return nullptr;
    }
    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (!building || load_at<int32_t>(building, 0) != 0) {
        return nullptr;
    }
    if (load_at<uint8_t>(level, 0xc9) != 0) {
        return nullptr;
    }
    if (call_vmethod<int32_t>(building, 0x60, false) != 1) {
        return nullptr;
    }
    return call_vmethod<void*>(building, 0x58);
}

extern "C" LIBG_EXPORT
void* _ZNK27LogicUpgradeBuildingCommand13getTargetDataEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return nullptr;
    }
    void* building = libg_get_game_object_by_id(load_at<int32_t>(self, 8));
    if (!building || load_at<int32_t>(building, 0) != 0) {
        return nullptr;
    }
    return call_vmethod<void*>(building, 0x58);
}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    _ZN12LogicCommand8destructEv(self);
    store_at<int32_t>(self, 8, 0);
}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommandC1Ei(void* self, int32_t a) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x10);
    store_at<int32_t>(self, 0, 0x1f8);
    store_at<int32_t>(self, 4, -1);
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
void _ZN31LogicSpeedUpConstructionCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 8));
    _ZN12LogicCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<uint32_t>(self, 8, read_u32(stream));
    _ZN12LogicCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand11getResourceEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return;
    }
    void* nested = load_at<void*>(self, 8);
    if (nested) {
        call_vmethod<void>(nested, 0x30, level);
        return;
    }
    (void)level_player_avatar(level);
    (void)libg_get_wallet_amount(static_cast<int32_t>(ResourceType::Gold));
}

extern "C" LIBG_EXPORT
int32_t _ZN24LogicBuyResourcesCommand16getResourceCountEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return 0;
    }
    void* avatar = level_player_avatar(level);
    const void* nested = load_at<void*>(self, 8);
    if (nested) {
        const int32_t target = call_vmethod<int32_t>(const_cast<void*>(nested), 0x2c, level);
        void* resource = call_vmethod<void*>(const_cast<void*>(nested), 0x30, level);
        const int32_t owned = avatar ? call_vmethod<int32_t>(avatar, 0x28, resource) : 0;
        return target - owned;
    }
    const int32_t attack_level = avatar ? call_vmethod<int32_t>(avatar, 0x20) : load_at<int32_t>(level, 0x48);
    int32_t cost = 100 * attack_level;
    if (load_at<uint8_t>(self, 0xc) != 0) {
        cost *= 5;
    }
    const int32_t owned = libg_get_wallet_amount(static_cast<int32_t>(ResourceType::Gold));
    return cost - owned;
}

extern "C" LIBG_EXPORT
int32_t _ZN24LogicBuyResourcesCommand19getMaxResourceCountEP10LogicLevel(void* self, void* level) {
    if (!self || !level) {
        return 0;
    }
    void* avatar = level_player_avatar(level);
    const void* nested = load_at<void*>(self, 8);
    if (nested) {
        return call_vmethod<int32_t>(const_cast<void*>(nested), 0x2c, level);
    }
    const int32_t attack_level = avatar ? call_vmethod<int32_t>(avatar, 0x20) : load_at<int32_t>(level, 0x48);
    int32_t cost = 100 * attack_level;
    if (load_at<uint8_t>(self, 0xc) != 0) {
        cost *= 5;
    }
    (void)avatar;
    return cost;
}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand8destructEv(void* self) {
    if (!self) {
        return;
    }
    void* nested = load_at<void*>(self, 8);
    if (nested) {
        std::free(nested);
    }
    _ZN12LogicCommand8destructEv(self);
    store_at<void*>(self, 8, nullptr);
    store_at<uint8_t>(self, 0xc, 0);
}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(void* self, void* base) {
    if (!self) {
        return;
    }
    std::memset(self, 0, 0x18);
    store_at<int32_t>(self, 0, 0x24f);
    store_at<int32_t>(self, 4, -1);
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
    void* nested = load_at<void*>(self, 8);
    const int32_t amount = _ZN24LogicBuyResourcesCommand16getResourceCountEP10LogicLevel(self, level);
    if (amount <= 0) {
        return nullptr;
    }
    if (!libg_spend_wallet_amount(static_cast<int32_t>(ResourceType::Diamonds), amount)) {
        return nullptr;
    }
    if (nested) {
        execute_known_command(nested, level);
    }
    return reinterpret_cast<void*>(1);
}

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicBuyResourcesCommand14getCommandTypeEv(void* self) { return 0x24f; }

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    if (load_at<void*>(self, 8) == nullptr) {
        write_flag(encoder, 0);
        write_flag(encoder, load_at<uint8_t>(self, 0xc));
    } else {
        write_flag(encoder, 1);
        void* nested = load_at<void*>(self, 8);
        if (nested) {
            _ZN19LogicCommandManager13encodeCommandEP15ChecksumEncoderP12LogicCommand(nullptr, encoder, nested);
        }
    }
    _ZN12LogicCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    const int32_t has_nested = read_flag(stream);
    if (has_nested == 1) {
        store_at<void*>(self, 8, _ZN19LogicCommandManager13decodeCommandEP10ByteStreambb(nullptr, stream, false, false));
    } else {
        void* nested = load_at<void*>(self, 8);
        if (nested) {
            std::free(nested);
        }
        store_at<void*>(self, 8, nullptr);
        store_at<uint8_t>(self, 0xc, static_cast<uint8_t>(read_flag(stream)));
    }
    _ZN12LogicCommand6decodeEP10ByteStream(self, stream);
}

// Base command classes recovered from the decompiled source.
extern "C" LIBG_EXPORT
void _ZN12LogicCommandC1Ev(void* self) {
    if (self) {
        store_at<int32_t>(self, 4, -1);
    }
}

extern "C" LIBG_EXPORT
void _ZN12LogicCommandC2Ev(void* self) {
    _ZN12LogicCommandC1Ev(self);
}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand8destructEv(void* self) {
    if (self) {
        store_at<int32_t>(self, 4, -1);
    }
}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    if (!self || !encoder) {
        return;
    }
    write_u32(encoder, load_at<uint32_t>(self, 4));
}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand6decodeEP10ByteStream(void* self, void* stream) {
    if (!self || !stream) {
        return;
    }
    store_at<uint32_t>(self, 4, read_u32(stream));
}

extern "C" LIBG_EXPORT
bool _ZNK12LogicCommand22isResourceCostingCommandEv(void* self) {
    (void)self;
    return false;
}

extern "C" LIBG_EXPORT
void* _ZNK12LogicCommand17getJSONForReplayEv(void* self) {
    if (!self) {
        return nullptr;
    }
    void* json = make_opaque_block(0x1c);
    if (!json) {
        return nullptr;
    }
    store_at<int32_t>(json, 0, load_at<int32_t>(self, 0));
    store_at<int32_t>(json, 4, load_at<int32_t>(self, 4));
    return json;
}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand13loadFromJSONEP15LogicJSONObject(void* self, void* json) {
    if (!self || !json) {
        return;
    }
    store_at<int32_t>(self, 0, load_at<int32_t>(json, 0));
    store_at<int32_t>(self, 4, load_at<int32_t>(json, 4));
}

extern "C" LIBG_EXPORT
int32_t _ZN12LogicCommand7executeEP10LogicLevel(void* self, void* level) {
    (void)self;
    (void)level;
    return 0;
}

extern "C" LIBG_EXPORT
int32_t _ZNK12LogicCommand14getCommandTypeEv(void* self) {
    return load_at<int32_t>(self, 0);
}

extern "C" LIBG_EXPORT
void _ZN12LogicCommand18setExecuteSubTickEi(void* self, int32_t sub_tick) {
    store_at<int32_t>(self, 4, sub_tick);
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
void _ZN27LogicResourceCostingCommandC1Ev(void* self) {
    _ZN12LogicCommandC1Ev(self);
}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommandC2Ev(void* self) {
    _ZN27LogicResourceCostingCommandC1Ev(self);
}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand8destructEv(void* self) {
    _ZN12LogicCommand8destructEv(self);
}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {
    _ZN12LogicCommand6encodeEP15ChecksumEncoder(self, encoder);
}

extern "C" LIBG_EXPORT
void _ZN27LogicResourceCostingCommand6decodeEP10ByteStream(void* self, void* stream) {
    _ZN12LogicCommand6decodeEP10ByteStream(self, stream);
}

extern "C" LIBG_EXPORT
bool _ZNK27LogicResourceCostingCommand22isResourceCostingCommandEv(void* self) {
    (void)self;
    return true;
}

extern "C" LIBG_EXPORT
int32_t _ZN27LogicResourceCostingCommand24getOneResourceCostAmountEP10LogicLevel(void* self, void* level) {
    (void)self;
    (void)level;
    return 0;
}

extern "C" LIBG_EXPORT
void* _ZN27LogicResourceCostingCommand22getOneResourceCostDataEP10LogicLevel(void* self, void* level) {
    (void)self;
    (void)level;
    return nullptr;
}

extern "C" LIBG_EXPORT
void* _ZNK27LogicResourceCostingCommand13getTargetDataEP10LogicLevel(void* self, void* level) {
    (void)self;
    (void)level;
    return nullptr;
}

extern "C" LIBG_EXPORT
int32_t _ZN27LogicResourceCostingCommand15getResourceCostEP10LogicLevel(void* self, void* level) {
    (void)self;
    (void)level;
    return 0;
}
