
#ifndef GAME_RAINBOW_H
#define GAME_RAINBOW_H

#include "../../../macro_magic/component.h"

COMPONENT(
    RainbowSpawner,
    HASH(0),
    FIELD_DEF_VAL(float, maxRainbowLength, 160)
)

    entt::entity rainbowEntity = entt::null;
    float currentLength = 0;
    vec2 prevPoint = vec2(0);

END_COMPONENT(RainbowSpawner)

#endif
