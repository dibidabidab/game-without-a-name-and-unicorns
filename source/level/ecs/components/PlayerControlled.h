
#ifndef GAME_PLAYERCONTROLLED_H
#define GAME_PLAYERCONTROLLED_H

#include "../../../macro_magic/component.h"

COMPONENT(
    PlayerControlled,
    HASH(playerId),
    FIELD_DEF_VAL(int, playerId, -1)
)END_COMPONENT(PlayerControlled)

#endif //GAME_PLAYERCONTROLLED_H
