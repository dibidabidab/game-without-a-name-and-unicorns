
#ifndef GAME_SINGLEPLAYERSESSION_H
#define GAME_SINGLEPLAYERSESSION_H


#include "Session.h"

class SingleplayerSession : public Session
{

    bool firstUpdate = true;

  public:

    SingleplayerSession(Level *lvl);

    void join(std::string username) override;

    void update(double deltaTime) override;

};


#endif
