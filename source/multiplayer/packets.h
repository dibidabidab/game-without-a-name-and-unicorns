
#ifndef GAME_PACKETS_H
#define GAME_PACKETS_H

#include "../macro_magic/json_reflectable.h"

#include "session/Player.h"
#include "../level/Level.h"

namespace Packet::from_player
{

REFLECTABLE_STRUCT(
    join_request,

    FIELD(std::string, name)

)END_REFLECTABLE_STRUCT(join_request)

}

namespace Packet
{

REFLECTABLE_STRUCT(
    entity_data_update,

    FIELD(int, roomI),
    FIELD(int, entityNetworkId),
    FIELD(int, dataTypeHash),
    FIELD(json, jsonData)
)END_REFLECTABLE_STRUCT(entity_data_update)

REFLECTABLE_STRUCT(
    entity_data_removed,

    FIELD(int, roomI),
    FIELD(int, entityNetworkId),
    FIELD(int, dataTypeHash)
)END_REFLECTABLE_STRUCT(entity_data_removed)

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

REFLECTABLE_STRUCT(
    entity_created,

    FIELD(int, roomI),
    FIELD(int, networkId),
    FIELD(int, entityTemplateHash)

)END_REFLECTABLE_STRUCT(entity_created)


REFLECTABLE_STRUCT(
    entity_destroyed,

    FIELD(int, roomI),
    FIELD(int, networkId)

)END_REFLECTABLE_STRUCT(entity_destroyed)

}

#endif //GAME_PACKETS_H
