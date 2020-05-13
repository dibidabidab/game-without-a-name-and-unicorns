
#ifndef GAME_LEG_H
#define GAME_LEG_H

#include "../../../macro_magic/component.h"

COMPONENT(
    Leg,
    HASH(0),
    FIELD_DEF_VAL(float, length, 24),
    FIELD_DEF_VAL(entt::entity, body, entt::null),
    FIELD_DEF_VAL(ivec2, anchor, ivec2(0)),
    FIELD_DEF_VAL(int, idleXPos, 0),

    FIELD_DEF_VAL(entt::entity, oppositeLeg, entt::null),

    FIELD_DEF_VAL(float, stepSize, 16),
    FIELD_DEF_VAL(float, stepArcAngle, 72),
    FIELD_DEF_VAL(float, idleStepSpeed, 65)
)

    vec2 target = vec2(0);

    bool moving = false;
    float distToTargetBeforeMoving = 0.;
    float timeSinceStartedMoving = 0.;
    float maxDistToTarget = 0.;

    vec2 moveAccumulator = vec2(0);

END_COMPONENT(Leg)

#endif //GAME_LEG_H
