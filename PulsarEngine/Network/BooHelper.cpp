#include <kamek.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/RKNet/ITEM.hpp>
#include <Network/PacketExpansion.hpp>

namespace Pulsar {
namespace Network {

// Get the network item for a remote player
ItemId GetNetworkPlayerItem(u8 playerId) 
{
    if (playerId >= 12) return ITEM_NONE;
    
    RKNet::ITEMHandler* handler = RKNet::ITEMHandler::sInstance;
    if (handler == nullptr) return ITEM_NONE;
    
    // Read from the received packet
    const RKNet::ITEMPacket& packet = handler->receivedPackets[playerId];
    ItemId storedItem = (ItemId)packet.storedItem;
    u8 mode = packet.mode;
    
    if (storedItem >= ITEM_MAX) return ITEM_NONE;

    if (mode == NET_ITEMSLOT_STATE_STOCK_1 || mode == NET_ITEMSLOT_STATE_STOCK_2 || mode == NET_ITEMSLOT_STATE_STOCK_3) 
        return storedItem;
    
    return ITEM_NONE;
}

// Get the network item count for a remote player
int GetNetworkPlayerItemCount(u8 playerId)
{
    if (playerId >= 12) return 0;
    
    RKNet::ITEMHandler* handler = RKNet::ITEMHandler::sInstance;
    if (handler == nullptr) return 0;
    
    // Read from the received packet
    const RKNet::ITEMPacket& packet = handler->receivedPackets[playerId];
    u8 mode = packet.mode;

    if (mode == NET_ITEMSLOT_STATE_STOCK_1) return 1;
    if (mode == NET_ITEMSLOT_STATE_STOCK_2) return 2;
    if (mode == NET_ITEMSLOT_STATE_STOCK_3) return 3;

    return 0;
}

} // namespace Network
} // namespace Pulsar