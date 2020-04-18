
#ifndef GAME_MULTIPLAYERSERVERSESSION_H
#define GAME_MULTIPLAYERSERVERSESSION_H

#include <atomic>
#include "MultiplayerSession.h"

class MultiplayerServerSession : public MultiplayerSession
{

    std::atomic_int playerIdCounter = 0;
    std::list<Player_ptr> playersJoining;
    std::vector<int> playersLeaving;
    std::mutex playersJoiningAndLeaving;

    SocketServer *server;

  public:

    MultiplayerServerSession(SocketServer *);

    bool isServer() const override { return true; }

    void update(double deltaTime) override;

    void setLevel(Level *);

    void join(std::string username) override;

    ~MultiplayerServerSession() override;

  private:

    bool handleJoinRequest(Player_ptr &player, Packet::from_player::join_request *, std::string &declineReason);

    void handleLeavingPlayers();

    void createPlayerEntity(Player_ptr &player);

    void removePlayerEntities(int playerId);

};


#endif
