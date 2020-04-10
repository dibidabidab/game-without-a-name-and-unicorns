
#ifndef GAME_MULTIPLAYERCLIENTSESSION_H
#define GAME_MULTIPLAYERCLIENTSESSION_H


#include "MultiplayerSession.h"

class MultiplayerClientSession : public MultiplayerSession
{

    MultiplayerIO io;

  public:

    std::function<void(const std::string &reason)> onJoinRequestDeclined = [](auto){};

    /**
     * NOTE: does not open the socket, so open the socket before or after constructing.
     */
    MultiplayerClientSession(SharedSocket);

    void join(std::string username);

    bool isServer() const override { return false; }

    void update(double deltaTime) override;
};


#endif
