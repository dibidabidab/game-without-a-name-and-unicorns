
#ifndef GAME_MULTIPLAYERSESSION_H
#define GAME_MULTIPLAYERSESSION_H


#include "io/MultiplayerIO.h"
#include "packets.h"
#include "../../../ecs/systems/networking/NetworkingSystem.h"
#include "../Session.h"

class MultiplayerSession : public Session
{
  public:

    MultiplayerSession(const char *saveGamePath) : Session(saveGamePath) {}

    virtual bool isServer() const = 0;

    template <class PacketType>
    void sendPacketToAllPlayers(PacketType &packet)
    {
        sendPacketToPlayers(packet, players);
    }

    template <class PacketType>
    void sendPacketToPlayers(PacketType &packet, std::list<Player_ptr> &players)
    {
        bool sentOnce = false;
        for (auto &p : players)
        {
            if (!sentOnce)
                sentOnce = sendPacketToPlayer(packet, p);
            else
                resendPacketToAnotherPlayer(p);
        }
    }

    template <class PacketType>
    bool sendPacketToPlayer(PacketType &packet, const Player_ptr &player)
    {
        if (!player->io) return false;

        player->io->send(packet, recentlySentPacket);

        return true;
    }

    void resendPacketToAnotherPlayer(const Player_ptr &player);

    virtual MultiplayerIO &getIOtoServer();

  protected:

    std::vector<NetworkingSystem *> networkingSystems;

    void addNetworkingSystemsToRooms();

  private:
    std::vector<char> recentlySentPacket;

};


#endif
