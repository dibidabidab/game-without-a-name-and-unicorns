
#ifndef GAME_AIMING_H
#define GAME_AIMING_H

#include "../../../macro_magic/component.h"

COMPONENT(
    Aiming,
    HASH(target.x, target.y),

    FIELD_DEF_VAL(ivec2, target, ivec2(0))
)
END_COMPONENT(Aiming)

#endif //GAME_AIMING_H
