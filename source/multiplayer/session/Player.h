
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "../../macro_magic/json_reflectable.h"
#include "../io/SocketServer.h"
#include "../io/MultiplayerIO.h"

REFLECTABLE_STRUCT(
    Player,

    FIELD         (int,         id),
    FIELD         (std::string, name)
)

    MultiplayerIO *io = NULL;

    Player() = default;

    Player(const Player &other) : id(other.id), name(other.name) {}

    ~Player()
    {
        delete io;
    }

END_REFLECTABLE_STRUCT(Player)

typedef std::shared_ptr<Player> Player_ptr;
typedef std::weak_ptr<Player> Player_weak_ptr;

#endif
