#include <kamek.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/Item/ItemPlayer.hpp>
#include <MarioKartWii/Driver/DriverManager.hpp>
#include <Extensions/ItemExpansion/ItemObjDrop.hpp>
#include <PulsarSystem.hpp>

// Expanded behaviourTable in mod BSS (from ItemSlotExpansion.cpp)
extern "C" Item::Behavior expandedBehaviourTable[27];

namespace Pulsar {
namespace Race {

static const s16 MUSHROOM_BOOST_DURATION = 90;

static s16 shroomStarCountdown[12] = {};

void SetShroomStarActive(u8 playerId) {
    if (playerId < 12) {
        shroomStarCountdown[playerId] = MUSHROOM_BOOST_DURATION;
    }
}

void UseShroomerStar(Item::Player& itemPlayer) {
    Kart::Movement* movement = itemPlayer.pointers->kartMovement;
    if (movement == nullptr) return;
    movement->ActivateMushroom();
    movement->ActivateStar();
    s16 mushroomDuration = movement->boost.mushroomBoostPanelFrames;
    if (mushroomDuration > 0) {
        movement->starTimer = mushroomDuration;
    }

    u8 playerId = itemPlayer.id;
    if (playerId < 12) {
        shroomStarCountdown[playerId] = mushroomDuration > 0 ? mushroomDuration : MUSHROOM_BOOST_DURATION;
    }

    itemPlayer.inventory.RemoveItems(1);

    if (DriverMgr::isOnlineRace && !itemPlayer.isRemote) {
        SendEncodedCustomUseEvent(OBJ_SHROOM_STAR, playerId);
    }
}

static void UpdateShroomStarStates() {
    Kart::Manager* kartMgr = Kart::Manager::sInstance;
    if (kartMgr == nullptr) return;

    for (u32 i = 0; i < 12; i++) {
        if (shroomStarCountdown[i] <= 0) continue;

        Kart::Player* kartPlayer = kartMgr->players[i];
        if (kartPlayer == nullptr) continue;

        Kart::Movement* movement = kartPlayer->pointers.kartMovement;
        if (movement == nullptr) continue;

        shroomStarCountdown[i]--;

        if (shroomStarCountdown[i] <= 0) {
            movement->starTimer = 0;
        } else if (movement->starTimer > shroomStarCountdown[i]) {
            movement->starTimer = shroomStarCountdown[i];
        }
    }
}

static void ResetShroomStarStates() {
    for (u32 i = 0; i < 12; i++) {
        shroomStarCountdown[i] = 0;
    }
}


void FusionMushroomBoost(Item::Player& itemPlayer) {
    Kart::Movement* movement = itemPlayer.pointers->kartMovement;
    if (movement != nullptr) {
        movement->ActivateMushroom();
    }
}


static void RegisterFusionItemBehaviours() {
    ResetShroomStarStates();

    Item::Behavior& shroomStar = expandedBehaviourTable[SHROOM_STAR];
    shroomStar.unknkown_0x0 = 1;
    shroomStar.unknkown_0x1 = 0;
    shroomStar.objId = OBJ_MUSHROOM;
    shroomStar.numberOfItems = 1;
    shroomStar.unknown_0xc = 0;
    shroomStar.unknown_0x10 = 0;
    shroomStar.useType = Item::ITEMUSE_USE;
    shroomStar.useFunction = UseShroomerStar;

    Item::Behavior& greenShellMush = expandedBehaviourTable[GREEN_SHELL_MUSHROOM];
    greenShellMush.unknkown_0x0 = 1;
    greenShellMush.unknkown_0x1 = 1;
    greenShellMush.objId = OBJ_GREEN_SHELL;
    greenShellMush.numberOfItems = 1;
    greenShellMush.unknown_0xc = 0;
    greenShellMush.unknown_0x10 = 0;
    greenShellMush.useType = Item::ITEMUSE_FIRE;
    greenShellMush.useFunction = FusionMushroomBoost;

    Item::Behavior& bobombMush = expandedBehaviourTable[BOBOMB_MUSHROOM];
    bobombMush.unknkown_0x0 = 1;
    bobombMush.unknkown_0x1 = 1;
    bobombMush.objId = OBJ_BOBOMB;
    bobombMush.numberOfItems = 1;
    bobombMush.unknown_0xc = 0;
    bobombMush.unknown_0x10 = 0;
    bobombMush.useType = Item::ITEMUSE_FIRE;
    bobombMush.useFunction = FusionMushroomBoost;
}
RaceLoadHook RegisterFusionItems(RegisterFusionItemBehaviours);

static void UpdateFusionItems() {
    UpdateShroomStarStates();
}
RaceFrameHook FusionItemsUpdate(UpdateFusionItems);

} // namespace Race
} // namespace Pulsar
