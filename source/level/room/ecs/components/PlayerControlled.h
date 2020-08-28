
#ifndef GAME_PLAYERCONTROLLED_H
#define GAME_PLAYERCONTROLLED_H

#include "../../../../macro_magic/component.h"

/**
 * Used to tell the game that an entity belongs to a player.
 */
COMPONENT(
    PlayerControlled,
    HASH(playerId),
    FIELD_DEF_VAL(final<int>, playerId, -1)
)END_COMPONENT(PlayerControlled)

struct LocalPlayer
{};

#endif //GAME_PLAYERCONTROLLED_H
