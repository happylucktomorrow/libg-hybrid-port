#pragma once

#include <cstdint>
#include <cstddef>

// Platform detection
#if defined(__ANDROID__)
#define LIBG_EXPORT __attribute__((visibility("default")))
#else
#define LIBG_EXPORT
#endif

// Game constants
constexpr int kMaxPlayers = 100;
constexpr int kMaxBuildings = 500;
constexpr int kMaxUnits = 200;

// Resource types
enum class ResourceType : int32_t {
    Gold = 0,
    Wood = 1,
    Stone = 2,
    Iron = 3,
    Diamonds = 4,
};

// Building types (subset)
enum class BuildingType : int32_t {
    Headquarters = 0,
    Sawmill = 1,
    Quarry = 2,
    GoldMine = 3,
    Armory = 4,
    Sculptor = 5,
    LandingCraft = 6,
    Gunboat = 7,
    Radar = 8,
    Vault = 9,
    Submarine = 10,
    RocketLauncher = 11,
    Cannon = 12,
    MachineGun = 13,
    Flamethrower = 14,
    Mortar = 15,
    ShockLauncher = 16,
};

// Game result codes
enum class GameResult : int32_t {
    Success = 0,
    Failure = -1,
    NotEnoughResources = -2,
    BuildingNotFound = -3,
    InvalidCommand = -4,
    CooldownActive = -5,
};

// Time
struct GameTime {
    int64_t seconds_since_epoch;
    int32_t day_of_week;   // 0=Monday
    int32_t hour_of_day;
};

// Resource wallet
struct ResourceWallet {
    int32_t gold;
    int32_t wood;
    int32_t stone;
    int32_t iron;
    int32_t diamonds;

    bool can_afford(ResourceType type, int32_t amount) const;
    void spend(ResourceType type, int32_t amount);
    void add(ResourceType type, int32_t amount);
};
