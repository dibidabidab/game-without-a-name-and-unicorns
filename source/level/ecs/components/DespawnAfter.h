
#ifndef GAME_DESPAWNAFTER_H
#define GAME_DESPAWNAFTER_H

#include "../../../macro_magic/component.h"

COMPONENT(
    DespawnAfter,
    HASH(time),
    FIELD(float, time),
    FIELD_DEF_VAL(float, timer, 0)
)
END_COMPONENT(DespawnAfter)

#endif //GAME_DESPAWNAFTER_H
