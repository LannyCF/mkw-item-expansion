#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <MarioKartWii/Item/Obj/Gesso.hpp>
#include <MarioKartWii/Driver/DriverManager.hpp>
#include <MarioKartWii/Input/InputManager.hpp>
#include <MarioKartWii/CourseMgr.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <MarioKartWii/Item/ItemSlot.hpp>
#include <Extensions/ItemExpansion/ItemObjDrop.hpp>
#include <Race/Boo.hpp>
#include <Race/Feather.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Race {

// To make remote player (other online players) use the expanded items on your screen
static int RemoteUseExpandedItem() 
{
    const Item::EVENTBuffer::Entry* entryPtr;
    asm volatile("mr %0, r31" : "=r"(entryPtr));
    const Item::EVENTBuffer::Entry& entry = *entryPtr;

    // Not expanded item
    if (entry.objId <= OBJ_NONE) return 0;

    const Item::UseEntry& useEntry = reinterpret_cast<const Item::UseEntry&>(entry);

    Item::Manager* mgr = Item::Manager::sInstance;
    Item::Player& player = mgr->players[Item::EVENTBuffer::GetUSEPlayerId(useEntry)];

    switch (entry.objId) 
    {
        case OBJ_BOO:
            UseBoo(player);
            break;
        case OBJ_FEATHER:
            UseFeather(player);
            break;
    }

    return 0;

}
kmCall(0x80796DF4, RemoteUseExpandedItem);

}//namespace Race
}//namespace Pulsar