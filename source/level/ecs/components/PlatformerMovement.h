
#ifndef GAME_PLATFORMERMOVEMENT_H
#define GAME_PLATFORMERMOVEMENT_H

#include "../../../macro_magic/component.h"

COMPONENT(
    PlatformerMovement,
    HASH(jump, left, right, fall, jumpVelocity, walkVelocity),

    FIELD_DEF_VAL(float, walkVelocity, 50),
    FIELD_DEF_VAL(float, jumpVelocity, 120),

    FIELD(bool, jump),
    FIELD(bool, left),
    FIELD(bool, right),
    FIELD(bool, fall)
)
    // used by system:
    float fallPressedTimer = 0;

END_COMPONENT(PlatformerMovement)

#endif
