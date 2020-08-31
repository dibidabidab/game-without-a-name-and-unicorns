#include <memory>
#include "MultiplayerSession.h"

void MultiplayerSession::addNetworkingSystemsToRooms()
{
    assert(level != NULL);
    networkingSystems.clear();
    for (int i = 0; i < level->getNrOfRooms(); i++)
    {
        auto sys = new NetworkingSystem(this);
        networkingSystems.push_back(sys);
        level->getRoom(i).addSystem(sys);
    }
}

void MultiplayerSession::resendPacketToAnotherPlayer(const Player_ptr &player)
{
    if (!player->io) return;
    player->io->socket->send(&recentlySentPacket[0], recentlySentPacket.size());
}

MultiplayerIO &MultiplayerSession::getIOtoServer()
{
    throw gu_err("This is not a client");
}
