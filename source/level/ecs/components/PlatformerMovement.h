
#ifndef GAME_PLATFORMERMOVEMENT_H
#define GAME_PLATFORMERMOVEMENT_H

#include "../../../macro_magic/component.h"

COMPONENT(
    PlatformerMovementInput,
    HASH(jump, fall, left, right),

    FIELD_DEF_VAL(bool, jump, false),
    FIELD_DEF_VAL(bool, fall, false),
    FIELD_DEF_VAL(bool, left, false),
    FIELD_DEF_VAL(bool, right, false)
)
END_COMPONENT(PlatformerMovementInput)

COMPONENT(
    PlatformerMovement,
    HASH(jumpVelocity, walkVelocity),

    FIELD_DEF_VAL(float, walkVelocity, 50),
    FIELD_DEF_VAL(float, jumpVelocity, 120)
)
    // used by system:
    float fallPressedTimer = 0;

END_COMPONENT(PlatformerMovement)

#endif
