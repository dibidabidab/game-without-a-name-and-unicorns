
#ifndef GAME_PACKETS_H
#define GAME_PACKETS_H

#include "../macro_magic/json_reflectable.h"

#include "session/Player.h"
#include "../level/Level.h"

namespace Packet::from_player
{

SERIALIZABLE(
    join_request,

    FIELD(std::string, name)

)END_SERIALIZABLE(join_request)

}

namespace Packet
{

SERIALIZABLE(
    entity_data_update,

    FIELD(int, roomI),
    FIELD(int, entityNetworkId),
    FIELD(int, dataTypeHash),
    FIELD(json, jsonData)
)END_SERIALIZABLE(entity_data_update)

SERIALIZABLE(
    entity_data_removed,

    FIELD(int, roomI),
    FIELD(int, entityNetworkId),
    FIELD(int, dataTypeHash)
)END_SERIALIZABLE(entity_data_removed)

}

namespace Packet::from_server
{

SERIALIZABLE(
    join_request_declined,

    FIELD(std::string, reason)

)END_SERIALIZABLE(join_request_declined)


SERIALIZABLE(
    join_request_accepted,

    FIELD(int, yourPlayerId),
    FIELD(std::vector<Player>, players)

)END_SERIALIZABLE(join_request_accepted)

SERIALIZABLE(
    player_joined,

    FIELD(Player, player)

)END_SERIALIZABLE(player_joined)

SERIALIZABLE(
    player_left,

    FIELD(int, playerId)

)END_SERIALIZABLE(player_left)

SERIALIZABLE(
    entity_created,

    FIELD(int, roomI),
    FIELD(int, networkId),
    FIELD(int, entityTemplateHash)

)END_SERIALIZABLE(entity_created)


SERIALIZABLE(
    entity_destroyed,

    FIELD(int, roomI),
    FIELD(int, networkId)

)END_SERIALIZABLE(entity_destroyed)

}

#endif //GAME_PACKETS_H
