
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

struct entity_data_update
{
    int roomI;
    int entityNetworkId;
    int dataTypeHash;
    json jsonData;
};

inline void to_json(json &j, const entity_data_update &u)
{
    j = json::array({u.roomI,
                     u.entityNetworkId,
                     u.dataTypeHash,
                     u.jsonData});
}

inline void from_json(const json &j, entity_data_update &u)
{
    u.roomI = j.at(0);
    u.entityNetworkId = j.at(1);
    u.dataTypeHash = j.at(2);
    u.jsonData = j.at(3);
}
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
    FIELD(int, networkId)

)END_REFLECTABLE_STRUCT(entity_created)

}

#endif //GAME_PACKETS_H
