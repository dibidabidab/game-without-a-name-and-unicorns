
#ifndef GAME_MULTIPLAYERCLIENTSESSION_H
#define GAME_MULTIPLAYERCLIENTSESSION_H


#include "MultiplayerSession.h"

class MultiplayerClientSession : public MultiplayerSession
{

    MultiplayerIO io;

  public:

    /**
     * NOTE: does not open the socket, so open the socket before or after constructing.
     */
    MultiplayerClientSession(SharedSocket);

    void join(std::string username) override;

    bool isServer() const override { return false; }

    void update(double deltaTime) override;
};


#endif
