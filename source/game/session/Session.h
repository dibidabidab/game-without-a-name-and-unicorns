
#ifndef GAME_SESSION_H
#define GAME_SESSION_H


#include "../../level/Level.h"
#include "Player.h"
#include "../SaveGame.h"

class Session
{
  protected:
    Level *level = NULL;

    std::list<Player_ptr> players;
    Player_ptr localPlayer;

    SaveGame saveGame;

  public:

    Session(const char *saveGamePath);

    virtual ~Session();

    std::function<void(const std::string &reason)> onJoinRequestDeclined = [](auto){};

    virtual void join(std::string username) = 0;


    delegate<void(Level *)> onNewLevel;

    Level *getLevel() const { return level; }


    virtual void update(double deltaTime) = 0;


    // todo: move player functions & members to a Players class.
    const Player_ptr &getLocalPlayer() const { return localPlayer; }

    const std::list<Player_ptr> &getPlayers() const { return players; }

    const Player_ptr &getPlayerById(int id) const;

    /**
     * Sets declineReason if name is not valid.
     */
    void validateUsername(const std::string &name, std::string &declineReason) const;


    Player_ptr deletePlayer(int id, std::list<Player_ptr> &players);

  protected:
    Player_ptr getPlayer(int id) const;

    Player_ptr deletePlayer(int id) { return deletePlayer(id, players); }

    void spawnPlayerEntities(bool networked=false);

    void spawnPlayerEntity(Player_ptr &, bool networked);

    void removePlayerEntities(int playerId);
};


#endif
