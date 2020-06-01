
#ifndef GAME_POLYPLATFORM_H
#define GAME_POLYPLATFORM_H

#include "../../../../macro_magic/component.h"

COMPONENT(
    PolyPlatform,
    HASH(allowFallThrough),
    FIELD_DEF_VAL(bool, allowFallThrough, true)
)

    std::vector<entt::entity> entitiesOnPlatform, entitiesAbovePlatform;

    ivec2 prevAABBPos = ivec2(0);

END_COMPONENT(PolyPlatform)

#endif //GAME_POLYPLATFORM_H
