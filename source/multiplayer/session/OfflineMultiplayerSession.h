
#ifndef GAME_OFFLINEMULTIPLAYERSESSION_H
#define GAME_OFFLINEMULTIPLAYERSESSION_H


#include "MultiplayerSession.h"

class OfflineMultiplayerSession : public MultiplayerSession
{

    bool firstUpdate = true;

  public:

    OfflineMultiplayerSession(Level *lvl);

    bool isServer() const override // yes but also no?
    {
        return true;
    }

    void join(std::string username) override;

    void update(double deltaTime) override;

};


#endif
