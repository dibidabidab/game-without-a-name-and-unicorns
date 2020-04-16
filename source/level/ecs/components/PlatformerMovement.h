
#ifndef GAME_PLATFORMERMOVEMENT_H
#define GAME_PLATFORMERMOVEMENT_H

#include "../../../macro_magic/component.h"

COMPONENT(JumpInput, HASH(0), FIELD(int, _))
END_COMPONENT(JumpInput)

COMPONENT(FallInput, HASH(0), FIELD(int, _))
END_COMPONENT(FallInput)

COMPONENT(WalkInput,
    HASH(left, right),
    FIELD_DEF_VAL(bool, left, false),
    FIELD_DEF_VAL(bool, right, false)
)END_COMPONENT(WalkInput)

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
