
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "../../macro_magic/serializable.h"
#include "../../game/session/multiplayer/io/SocketServer.h"
#include "../../game/session/multiplayer/io/MultiplayerIO.h"

SERIALIZABLE(
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

END_SERIALIZABLE(Player)

typedef std::shared_ptr<Player> Player_ptr;

#endif
