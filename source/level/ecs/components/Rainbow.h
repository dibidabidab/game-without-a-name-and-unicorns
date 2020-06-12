
#ifndef GAME_RAINBOW_H
#define GAME_RAINBOW_H

#include "../../../macro_magic/component.h"

COMPONENT(
    Rainbow,
    HASH(0),
    FIELD_DEF_VAL(bool, disappearOnPlayerTouch, false),
    FIELD_DEF_VAL(float, disappearAfterTime, 0), // 0 means never.
    FIELD_DEF_VAL(bool, startDissapearingFromLeft, true),
    FIELD_DEF_VAL(float, disappearSpeed, 1)
)

    bool disappearing = false;
    float timer = 0;

END_COMPONENT(Rainbow)

COMPONENT(
    RainbowSpawner,
    HASH(0),
    FIELD_DEF_VAL(float, maxRainbowLength, 160),
    FIELD(Rainbow, rainbowSettings)
)

    entt::entity rainbowEntity = entt::null;
    float currentLength = 0;
    ivec2 prevPoint = ivec2(0);

END_COMPONENT(RainbowSpawner)

#endif
