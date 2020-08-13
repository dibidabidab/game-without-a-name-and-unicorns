
#ifndef GAME_POLYPLATFORM_H
#define GAME_POLYPLATFORM_H

#include "../../../../macro_magic/component.h"
#include "../Polyline.h"

COMPONENT(
    PolyPlatform,
    HASH(allowFallThrough),
    FIELD_DEF_VAL(bool, allowFallThrough, true),
    FIELD_DEF_VAL(TileMaterial, material, 0)
)

    std::vector<entt::entity> entitiesOnPlatform, entitiesAbovePlatform;

    ivec2 prevAABBPos = ivec2(0);

END_COMPONENT(PolyPlatform)


COMPONENT(
    PolylineWaves,
    HASH(stiffness, dampening, spread),
    FIELD_DEF_VAL(float, stiffness, 240),
    FIELD_DEF_VAL(float, dampening, 4),
    FIELD_DEF_VAL(float, spread, 45),
    FIELD_DEF_VAL(float, impactMultiplier, 1),
    FIELD_DEF_VAL(float, moveByWind, 0)
)

    struct Spring
    {
        float
            yOffset = 0, velocity = 0;
    };

    std::vector<Spring> springs;

END_COMPONENT(PolylineWaves)

#endif //GAME_POLYPLATFORM_H
