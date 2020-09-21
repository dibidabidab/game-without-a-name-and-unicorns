
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "../../macro_magic/serializable.h"
#include "../../game/session/multiplayer/io/SocketServer.h"
#include "../../game/session/multiplayer/io/MultiplayerIO.h"
#include "../../generated/Player.hpp"


typedef std::shared_ptr<Player> Player_ptr;

#endif
