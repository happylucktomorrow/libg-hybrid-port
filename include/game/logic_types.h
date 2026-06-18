#pragma once

#include "game/libg_api.h"
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <functional>

// Forward declarations for game logic types.
// These match the original class hierarchy to maintain ABI compatibility.

class LogicGameObject;
class LogicBuilding;
class LogicObstacle;
class LogicComponent;
class LogicCommand;
class LogicCommandManager;
class LogicGameObjectManager;
class LogicClientAvatar;
class LogicClientHome;
class LogicGameListener;
class LogicData;
class LogicDataTable;

// Game object types
enum class LogicGameObjectType : int32_t {
    Building = 0,
    Obstacle = 1,
    Deco = 2,
    Trap = 3,
    ResourceShip = 4,
    LandingShip = 5,
    Character = 6,
    Spell = 7,
    Projectile = 8,
    LootBox = 9,
    Effect = 10,
    AnimSpawner = 11,
    Shield = 12,
    Cannon = 13,
    Mortar = 14,
    // ...
};

// Resource types
enum class LogicResourceType : int32_t {
    Gold = 0,
    Wood = 1,
    Stone = 2,
    Iron = 3,
    Diamonds = 4,
    Energy = 5,
};

// Command types
enum class LogicCommandType : int32_t {
    BuyBuilding = 1,
    MoveBuilding = 2,
    UpgradeBuilding = 3,
    SellBuilding = 4,
    TrainUnit = 5,
    BuyDeco = 6,
    ClearObstacle = 7,
    FreeWorker = 8,
    SpeedUpConstruction = 9,
    SpeedUpTraining = 10,
    BuyResources = 11,
    JoinAlliance = 12,
    LeaveAlliance = 13,
    ChangeAvatarName = 14,
    BoostArtifact = 15,
    ClaimAchievement = 16,
    StartMission = 17,
    EndCombat = 18,
    // ...
};

// Building data
struct BuildingData {
    int32_t building_id;
    BuildingType type;
    std::string name;
    int32_t max_level;
    int32_t width;
    int32_t height;
    int32_t build_cost_gold;
    int32_t build_cost_wood;
    int32_t build_cost_stone;
    int32_t build_time_seconds;
    int32_t required_hq_level;
    int32_t experience_reward;
};

// Command result
struct CommandResult {
    bool success;
    int32_t error_code;
    std::string error_message;
};
