
#ifndef GAME_PLATFORMERMOVEMENT_H
#define GAME_PLATFORMERMOVEMENT_H

#include "../../../macro_magic/component.h"

//struct PlatformerMovement
//{
//    float jumpVelocity = 120;
//    float walkVelocity = 50;
//
//    bool jump = false, left = false, right = false, fall = false;
//
//    // used by system:
//    float fallPressedTimer = 0;
//};

COMPONENT(
    PlatformerMovement,
    HASH(jump, left, right, fall, jumpVelocity, walkVelocity),

    FIELD_DEF_VAL(float, jumpVelocity, 120),
    FIELD_DEF_VAL(float, walkVelocity, 50),

    FIELD(bool, jump),
    FIELD(bool, left),
    FIELD(bool, right),
    FIELD(bool, fall),

    FIELD_DEF_VAL(float, fallPressedTimer, 0)
)

// todo: move this to other file?:
struct LocalPlayer {};

#endif
