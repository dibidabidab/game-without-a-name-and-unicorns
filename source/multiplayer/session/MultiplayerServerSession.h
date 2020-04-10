
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

    void setLevel(Level *);

  private:
    template <class PacketType>
    void sendPacketToAllPlayers(PacketType &packet)
    {
        for (auto &p : players)
            if (p->io) p->io->send(packet); // wait what.. todo: only serialize packet once, instead for each player
    }

    void handleJoinRequest(Player *, Packet::from_player::join_request *);

    void handleLeavingPlayers();

};


#endif
