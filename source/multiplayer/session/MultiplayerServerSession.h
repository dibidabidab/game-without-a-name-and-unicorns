
#ifndef GAME_MULTIPLAYERSERVERSESSION_H
#define GAME_MULTIPLAYERSERVERSESSION_H


#include "MultiplayerSession.h"

class MultiplayerServerSession : public MultiplayerSession
{

    int playerIdCounter = 0;
    std::vector<Player_ptr> playersJoining;
    std::vector<int> playersLeaving;
    std::mutex playersJoiningAndLeaving;

    SocketServer *server;

  public:

    MultiplayerServerSession(SocketServer *);

    bool isServer() const override { return true; }

    void update(double deltaTime) override;

  private:
    template <class PacketType>
    void sendPacketToAllPlayers(PacketType &packet)
    {
        for (auto &p : players)
            if (p->io) p->io->send(packet);
    }

    void handleJoinRequest(Player *, Packet::from_player::join_request *);

};


#endif
