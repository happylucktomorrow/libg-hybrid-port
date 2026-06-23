#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace {

struct OpaqueLogicCommand {
    int32_t type = 0;
    bool executed = false;
};

OpaqueLogicCommand* make_command(int32_t type) {
    auto* cmd = static_cast<OpaqueLogicCommand*>(std::calloc(1, sizeof(OpaqueLogicCommand)));
    if (!cmd) {
        return nullptr;
    }
    cmd->type = type;
    return cmd;
}

} // anonymous namespace

// ============================================================================
// Game command implementations
// These replace the corresponding Logic*Command methods from the original libg.
// Each exported symbol matches the original mangled name exactly.
// ============================================================================

// Example: LogicBuyBuildingCommand::execute
// Original mangled: _ZN21LogicBuyBuildingCommand7executeEv
extern "C" LIBG_EXPORT
void _ZN21LogicBuyBuildingCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicTrainUnitCommand::execute
extern "C" LIBG_EXPORT
void _ZN20LogicTrainUnitCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicMoveBuildingCommand::execute
extern "C" LIBG_EXPORT
void _ZN21LogicMoveBuildingCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicUpgradeBuildingCommand::execute
extern "C" LIBG_EXPORT
void _ZN25LogicUpgradeBuildingCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicSellBuildingCommand::execute
extern "C" LIBG_EXPORT
void _ZN21LogicSellBuildingCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicClearObstacleCommand::execute
extern "C" LIBG_EXPORT
void _ZN24LogicClearObstacleCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicBuyResourcesCommand::execute
extern "C" LIBG_EXPORT
void _ZN21LogicBuyResourcesCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
    }
}

// Example: LogicSpeedUpConstructionCommand::execute
extern "C" LIBG_EXPORT
void _ZN28LogicSpeedUpConstructionCommand7executeEv(void* self) {
    if (auto* cmd = static_cast<OpaqueLogicCommand*>(self)) {
        cmd->executed = true;
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

    // The real game has many more checks here; this keeps the port coherent
    // until the original logic is fully reconstructed.
    return true;
}

// ============================================================================
// Command factory
// ============================================================================

extern "C" LIBG_EXPORT
void* _ZN20LogicCommandFactory9createCommandEN5Logic12CommandTypeE(int32_t type) {
    return make_command(type);
}
