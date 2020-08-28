
#ifndef GAME_MULTIPLAYERSESSION_H
#define GAME_MULTIPLAYERSESSION_H


#include "Player.h"
#include "../io/MultiplayerIO.h"
#include "../packets.h"
#include "../../level/Level.h"
#include "../../level/room/ecs/systems/networking/NetworkingSystem.h"

class MultiplayerSession
{
  protected:

    Level *level = NULL;

    std::list<Player_ptr> players;

    Player_ptr localPlayer;

    MultiplayerSession() = default;

  public:

    std::function<void(const std::string &reason)> onJoinRequestDeclined = [](auto){};

    std::function<void(Level *)> onNewLevel;

    Level *getLevel() const { return level; }

    virtual bool isServer() const = 0;

    virtual void update(double deltaTime) = 0;

    const Player_ptr &getLocalPlayer() const { return localPlayer; }

    const std::list<Player_ptr> &getPlayers() const { return players; }

    const Player_ptr &getPlayerById(int id) const;

    virtual void join(std::string username) = 0;

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

    Player_ptr deletePlayer(int id, std::list<Player_ptr> &players);

    virtual ~MultiplayerSession() = default;

  protected:

    /**
     * Sets declineReason if name is not valid.
     */
    void validateUsername(const std::string &name, std::string &declineReason) const;

    Player_ptr deletePlayer(int id) { return deletePlayer(id, players); }

    Player_ptr getPlayer(int id) const;

    std::vector<NetworkingSystem *> networkingSystems;

    void addNetworkingSystemsToRooms();

  private:
    std::vector<char> recentlySentPacket;

};


#endif
