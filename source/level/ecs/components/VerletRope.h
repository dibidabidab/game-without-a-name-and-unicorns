
#ifndef GAME_VERLETROPE_H
#define GAME_VERLETROPE_H

#include "../../../macro_magic/component.h"

#include <vector>

struct RopePoint
{
    vec2 currentPos, prevPos;
};

COMPONENT(
    VerletRope,

    HASH(length, nrOfPoints, gravity.x, gravity.y, friction),

    FIELD_DEF_VAL(float, length, 32),
    FIELD_DEF_VAL(uint8, nrOfPoints, 16),
    FIELD_DEF_VAL(vec2, gravity, ivec2(0, -1)),
    FIELD_DEF_VAL(float, friction, .999), // magic number. Gives best results imo

    FIELD_DEF_VAL(entt::entity, endPointEntity, entt::null),
    FIELD_DEF_VAL(bool, fixedEndPoint, false)
)

    std::vector<RopePoint> points;

END_COMPONENT(VerletRope)

COMPONENT(
    AttachToRope,
    HASH(ropeEntity),

    FIELD_DEF_VAL(entt::entity, ropeEntity, entt::null),
    FIELD_DEF_VAL(float, x, 1) // where along the rope the entity should stick to. 0 = start, 1 = end
)
END_COMPONENT(AttachToRope)

#endif //GAME_VERLETROPE_H
