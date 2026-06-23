#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstring>

// ============================================================================
// Game command implementations.
// The recovered original behavior is mostly empty/constant-return methods for
// this slice, so we mirror that instead of inventing gameplay logic.
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN21LogicBuyBuildingCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN20LogicTrainUnitCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN21LogicMoveBuildingCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN25LogicUpgradeBuildingCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN21LogicSellBuildingCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN24LogicClearObstacleCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN21LogicBuyResourcesCommand7executeEv(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN28LogicSpeedUpConstructionCommand7executeEv(void* self) {
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
void _ZN23LogicBuyBuildingCommandC1EP17LogicBuildingDataiib(void* self, void* data, int32_t a, int32_t b, bool c) {}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommandC2EP17LogicBuildingDataiib(void* self, void* data, int32_t a, int32_t b, bool c) {}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void _ZN23LogicBuyBuildingCommand12executeBuildEP10LogicLevelP13LogicBuildingS3_S3_(void* self, void* level, void* p1, void* p2, void* p3) {}

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
void _ZN21LogicTrainUnitCommandC1EiP19LogicCombatItemData(void* self, int32_t a, void* data) {}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommandC2EiP19LogicCombatItemData(void* self, int32_t a, void* data) {}

extern "C" LIBG_EXPORT
void _ZN21LogicTrainUnitCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
int32_t _ZNK21LogicTrainUnitCommand14getCommandTypeEv(void* self) { return 0; }

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
void _ZN24LogicMoveBuildingCommandC1Eiii(void* self, int32_t a, int32_t b, int32_t c) {}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommandC2Eiii(void* self, int32_t a, int32_t b, int32_t c) {}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicMoveBuildingCommand14getCommandTypeEv(void* self) { return 0; }

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN24LogicMoveBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommandC1Ei(void* self, int32_t a) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommandC2Ei(void* self, int32_t a) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicSellBuildingCommand14getCommandTypeEv(void* self) { return 0; }

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN24LogicSellBuildingCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommandC1Ei(void* self, int32_t a) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommandC2Ei(void* self, int32_t a) {}

extern "C" LIBG_EXPORT
void _ZN25LogicClearObstacleCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
int32_t _ZNK25LogicClearObstacleCommand14getCommandTypeEv(void* self) { return 0; }

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
void _ZN27LogicUpgradeBuildingCommandC1Eib(void* self, int32_t a, bool b) {}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommandC2Eib(void* self, int32_t a, bool b) {}

extern "C" LIBG_EXPORT
void _ZN27LogicUpgradeBuildingCommand7executeEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
int32_t _ZNK27LogicUpgradeBuildingCommand14getCommandTypeEv(void* self) { return 0; }

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
void _ZN31LogicSpeedUpConstructionCommandC1Ei(void* self, int32_t a) {}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommandC2Ei(void* self, int32_t a) {}

extern "C" LIBG_EXPORT
void* _ZN31LogicSpeedUpConstructionCommand7executeEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
int32_t _ZNK31LogicSpeedUpConstructionCommand14getCommandTypeEv(void* self) { return 0; }

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand6encodeEP15ChecksumEncoder(void* self, void* encoder) {}

extern "C" LIBG_EXPORT
void _ZN31LogicSpeedUpConstructionCommand6decodeEP10ByteStream(void* self, void* stream) {}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand11getResourceEP10LogicLevel(void* self, void* level) {}

extern "C" LIBG_EXPORT
void* _ZN24LogicBuyResourcesCommand16getResourceCountEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void* _ZN24LogicBuyResourcesCommand19getMaxResourceCountEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommand8destructEv(void* self) {}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommandC1EP27LogicResourceCostingCommand(void* self, void* base) {}

extern "C" LIBG_EXPORT
void _ZN24LogicBuyResourcesCommandC2EP27LogicResourceCostingCommand(void* self, void* base) {}

extern "C" LIBG_EXPORT
void* _ZN24LogicBuyResourcesCommand7executeEP10LogicLevel(void* self, void* level) { return nullptr; }

extern "C" LIBG_EXPORT
int32_t _ZNK24LogicBuyResourcesCommand14getCommandTypeEv(void* self) { return 0; }

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
