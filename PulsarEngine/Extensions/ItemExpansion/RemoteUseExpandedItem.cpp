#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <MarioKartWii/Item/Obj/Gesso.hpp>
#include <MarioKartWii/Item/Obj/ItemEVENT.hpp>
#include <MarioKartWii/Driver/DriverManager.hpp>
#include <MarioKartWii/Input/InputManager.hpp>
#include <MarioKartWii/CourseMgr.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <MarioKartWii/Item/ItemSlot.hpp>
#include <MarioKartWii/Item/Obj/ItemObjHolder.hpp>
#include <MarioKartWii/RKNet/PacketMgr.hpp>
#include <Extensions/ItemExpansion/ItemObjDrop.hpp>
#include <Race/Boo.hpp>
#include <Race/Feather.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Race {

static const u8 CUSTOM_EVENT_MAGIC = 0xFF;

void SendEncodedCustomUseEvent(ItemObjId realObjId, u8 playerId) {
    u8 encodedData[3];
    encodedData[0] = CUSTOM_EVENT_MAGIC;
    encodedData[1] = (u8)realObjId;
    encodedData[2] = playerId;

    RKNet::PacketMgr* mgr = RKNet::PacketMgr::sInstance;
    if (mgr != nullptr && mgr->GetEVENTFreeDataSpace() >= 3 && mgr->HasFreeEVENTEntries()) {
        mgr->AddEVENTEntry(OBJ_MUSHROOM, RKNet::EVENTACTION_USE, encodedData, 3);
    } else {
        Item::EVENTBuffer* evtBuf = Item::EVENTBuffer::sInstance;
        if (evtBuf != nullptr) {
            evtBuf->QueueSendEntry(OBJ_MUSHROOM, RKNet::EVENTACTION_USE, encodedData, 3);
        }
    }
}

static bool InterceptCustomRecvEVENT(Item::ObjHolder* holder, RKNet::EVENTAction action, u16 unk2,
                                      const Item::EVENTBuffer::Entry& entry, u32 time, bool flag) {
    ItemObjId objId = entry.objId;

    if (objId == OBJ_MUSHROOM && action == RKNet::EVENTACTION_USE
        && entry.data[0] == CUSTOM_EVENT_MAGIC) {
        ItemObjId realObjId = (ItemObjId)entry.data[1];
        u8 playerId = entry.data[2];
        if (realObjId > OBJ_NONE) {
            Item::Manager* mgr = Item::Manager::sInstance;
            if (mgr == nullptr || playerId >= mgr->playerCount) return false;
            Item::Player& player = mgr->players[playerId];
            switch (realObjId) {
                case OBJ_BOO:
                    ApplyBooRemoteEffect(playerId);
                    break;
                case OBJ_FEATHER:
                    ApplyFeatherRemoteEffect(player);
                    break;
                case OBJ_SHROOM_STAR:
                    SetShroomStarActive(playerId);
                    break;
                default:
                    break;
            }
            return false;
        }
    }

    if (objId > OBJ_NONE && action == RKNet::EVENTACTION_USE) {
        const Item::UseEntry& useEntry = reinterpret_cast<const Item::UseEntry&>(entry);
        Item::Manager* mgr = Item::Manager::sInstance;
        if (mgr == nullptr) return false;
        u8 playerId = Item::EVENTBuffer::GetUSEPlayerId(useEntry);
        if (playerId >= mgr->playerCount) return false;
        Item::Player& player = mgr->players[playerId];
        switch (objId) {
            case OBJ_BOO:
                ApplyBooRemoteEffect(playerId);
                break;
            case OBJ_FEATHER:
                ApplyFeatherRemoteEffect(player);
                break;
            default:
                break;
        }
        return false;
    }
    if (objId > OBJ_NONE) {
        return false;
    }
    return holder->ProcessRecvEVENT(action, unk2, entry, time, flag);
}
kmCall(0x8079bf88, InterceptCustomRecvEVENT);

}//namespace Race
}//namespace Pulsar