
#ifndef GAME_BLOODDROP_H
#define GAME_BLOODDROP_H

#include "../../../../macro_magic/component.h"

COMPONENT(
    BloodDrop,
    HASH(0),
    FIELD_DEF_VAL(float, size, 5),
    FIELD_DEF_VAL(uint8, color, 10u),
    FIELD_DEF_VAL(bool, permanentDraw, false)
)
    float timer = 0, splitAtTime = 0, drawPermanentAfterTime = 0;
END_COMPONENT(BloodDrop)

#endif //GAME_BLOODDROP_H
