
#ifndef GAME_MULTIPLAYERSESSION_H
#define GAME_MULTIPLAYERSESSION_H


#include "Player.h"
#include "../io/MultiplayerIO.h"
#include "../packets.h"
#include "../../level/Level.h"

class MultiplayerSession
{
  protected:

    Level *level = NULL;

    std::vector<Player_ptr> players;

    Player_ptr localPlayer;

    MultiplayerSession() = default;

  public:

    std::function<void(Level *)> onNewLevel;

    Level *getLevel() const { return level; }

    virtual bool isServer() const = 0;

    virtual void update(double deltaTime) = 0;

    const Player_ptr getLocalPlayer() const { return localPlayer; }
    const std::vector<Player_ptr> getPlayers() const { return players; }

  protected:

    /**
     * Sets declineReason if name is not valid.
     */
    void validateUsername(const std::string &name, std::string &declineReason) const;

    Player_ptr deletePlayer(int id) { return deletePlayer(id, players); }

    Player_ptr deletePlayer(int id, std::vector<Player_ptr> &players);

    Player_ptr getPlayer(int id) const;

};


#endif
