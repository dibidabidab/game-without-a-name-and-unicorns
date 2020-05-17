
#ifndef GAME_LIGHT_H
#define GAME_LIGHT_H

#include "../../../macro_magic/component.h"

/**
 * Any entity with LightPoint + AABB will create a light that (optionally) casts shadows.
 */
COMPONENT(
    LightPoint,
    HASH(radius, castShadow),

    FIELD_DEF_VAL(uint8, radius, 128),
    FIELD_DEF_VAL(bool, castShadow, true)
)

    ivec2 prevPosition;
    int shadowTextureIndex = -1;
    int prevRadius = -1;

END_COMPONENT(LightPoint)

#endif //GAME_LIGHT_H
