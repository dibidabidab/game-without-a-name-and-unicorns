
#ifndef GAME_MULTIPLAYERSESSION_H
#define GAME_MULTIPLAYERSESSION_H


#include "Player.h"
#include "../io/MultiplayerIO.h"
#include "../packets.h"

class MultiplayerSession
{
  protected:

    std::vector<Player_ptr> players;

    MultiplayerSession() = default;

  public:

    virtual bool isServer() const = 0;

    virtual void update(double deltaTime) = 0;

  protected:

    /**
     * Sets declineReason if name is not valid.
     */
    void validateUsername(const std::string &name, std::string &declineReason) const;

    Player_ptr deletePlayer(int id);

    Player_ptr getPlayer(int id);

};


#endif
