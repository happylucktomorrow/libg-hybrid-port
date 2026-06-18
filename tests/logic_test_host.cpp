#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

// Include the game logic headers
#include "game/logic_types.h"
#include "game/libg_api.h"

// Mock implementations for host testing
extern "C" void _ZN21LogicBuyBuildingCommand7executeEv(void*);
extern "C" bool _ZN21LogicBuyBuildingCommand6canExecuteEPN5Logic5StateE(void*, void*);
extern "C" void _ZN20LogicTrainUnitCommand7executeEv(void*);
extern "C" void _ZN22LogicGameObjectManagerC1Ev(void*);
extern "C" void _ZN18LogicCommandManagerC1Ev(void*);
extern "C" bool _ZN18LogicCommandManager11executeCommandEPN5Logic7CommandE(void*, void*);

int main() {
    printf("=== libg-hybrid-port host test ===\n\n");

    printf("Testing ResourceWallet...\n");
    ResourceWallet wallet;
    wallet.gold = 1000;
    wallet.wood = 500;
    wallet.stone = 300;
    wallet.iron = 200;
    wallet.diamonds = 10;

    assert(wallet.can_afford(ResourceType::Gold, 100));
    printf("  can_afford(gold, 100): PASS\n");

    wallet.spend(ResourceType::Gold, 100);
    assert(wallet.gold == 900);
    printf("  spend(gold, 100): PASS (gold now %d)\n", wallet.gold);

    wallet.add(ResourceType::Gold, 500);
    assert(wallet.gold == 1400);
    printf("  add(gold, 500): PASS (gold now %d)\n", wallet.gold);

    printf("\nTesting GameObjectManager...\n");
    char obj_mgr_buf[128] = {};
    _ZN22LogicGameObjectManagerC1Ev(obj_mgr_buf);
    printf("  GameObjectManager created: PASS\n");

    printf("\nTesting CommandManager...\n");
    char cmd_mgr_buf[128] = {};
    _ZN18LogicCommandManagerC1Ev(cmd_mgr_buf);
    printf("  CommandManager created: PASS\n");

    printf("\nTesting BuyBuildingCommand...\n");
    char cmd_buf[256] = {};
    _ZN21LogicBuyBuildingCommand7executeEv(cmd_buf);
    printf("  BuyBuildingCommand::execute: PASS (no crash)\n");

    bool can = _ZN21LogicBuyBuildingCommand6canExecuteEPN5Logic5StateE(cmd_buf, nullptr);
    printf("  BuyBuildingCommand::canExecute: %s\n", can ? "true" : "false");

    printf("\nTesting TrainUnitCommand...\n");
    _ZN20LogicTrainUnitCommand7executeEv(cmd_buf);
    printf("  TrainUnitCommand::execute: PASS (no crash)\n");

    printf("\n=== ALL TESTS PASSED ===\n");
    printf("\nThis is host-side only. For real testing, build for Android\n");
    printf("and push libg.so to a device.\n");
    return 0;
}
