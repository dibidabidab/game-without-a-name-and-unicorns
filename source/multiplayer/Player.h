
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "../macro_magic/json_reflectable.h"

REFLECTABLE_STRUCT(
    Player,

    FIELD         (std::string, name),
    FIELD_DEF_VAL (int,         ping, 0)
)
END_REFLECTABLE_STRUCT(Player)

#endif
