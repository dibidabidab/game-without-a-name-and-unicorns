
#ifndef GAME_PACKETS_H
#define GAME_PACKETS_H

#include "../macro_magic/json_reflectable.h"

#include "session/Player.h"

namespace Packet::from_player
{

REFLECTABLE_STRUCT(
    join_request,

    FIELD(std::string, name)

)END_REFLECTABLE_STRUCT(join_request)

}

namespace Packet::from_server
{

REFLECTABLE_STRUCT(
    join_request_declined,

    FIELD(std::string, reason)

)END_REFLECTABLE_STRUCT(join_request_declined)


REFLECTABLE_STRUCT(
    join_request_accepted,

    FIELD(int, yourPlayerId),
    FIELD(std::vector<Player>, players)

)END_REFLECTABLE_STRUCT(join_request_accepted)

REFLECTABLE_STRUCT(
    player_joined,

    FIELD(Player, player)

)END_REFLECTABLE_STRUCT(player_joined)

REFLECTABLE_STRUCT(
    player_left,

    FIELD(int, playerId)

)END_REFLECTABLE_STRUCT(player_left)

}

#endif //GAME_PACKETS_H
