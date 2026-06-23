#include "game/libg_api.h"

#include <algorithm>

bool ResourceWallet::can_afford(ResourceType type, int32_t amount) const {
    if (amount < 0) {
        return false;
    }

    switch (type) {
    case ResourceType::Gold: return gold >= amount;
    case ResourceType::Wood: return wood >= amount;
    case ResourceType::Stone: return stone >= amount;
    case ResourceType::Iron: return iron >= amount;
    case ResourceType::Diamonds: return diamonds >= amount;
    }

    return false;
}

void ResourceWallet::spend(ResourceType type, int32_t amount) {
    if (amount <= 0) {
        return;
    }

    auto clamp_sub = [amount](int32_t value) {
        return value > amount ? value - amount : 0;
    };

    switch (type) {
    case ResourceType::Gold: gold = clamp_sub(gold); break;
    case ResourceType::Wood: wood = clamp_sub(wood); break;
    case ResourceType::Stone: stone = clamp_sub(stone); break;
    case ResourceType::Iron: iron = clamp_sub(iron); break;
    case ResourceType::Diamonds: diamonds = clamp_sub(diamonds); break;
    }
}

void ResourceWallet::add(ResourceType type, int32_t amount) {
    if (amount <= 0) {
        return;
    }

    auto clamp_add = [amount](int32_t value) {
        const int64_t sum = static_cast<int64_t>(value) + static_cast<int64_t>(amount);
        return static_cast<int32_t>(std::min<int64_t>(sum, INT32_MAX));
    };

    switch (type) {
    case ResourceType::Gold: gold = clamp_add(gold); break;
    case ResourceType::Wood: wood = clamp_add(wood); break;
    case ResourceType::Stone: stone = clamp_add(stone); break;
    case ResourceType::Iron: iron = clamp_add(iron); break;
    case ResourceType::Diamonds: diamonds = clamp_add(diamonds); break;
    }
}
