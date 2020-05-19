
#ifndef GAME_LEG_H
#define GAME_LEG_H

#include "../../../../macro_magic/component.h"

/**
 * A leg that moves using procedural animation.
 *
 * body = e.g. the player entity
 * anchor = the position of the hip on the body.
 */
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
    FIELD_DEF_VAL(float, idleStepSpeed, 65),
    FIELD_DEF_VAL(float, inAirStepSpeed, 40)
)

    // variables used by the LegsSystem:

    bool initialized = false;
    vec2 target = vec2(0);

    bool moving = false, shouldBeMoving = false;
    float distToTargetBeforeMoving = 0.;
    float timeSinceStartedMoving = 0.;
    float maxDistToTarget = 0.;

    vec2 moveAccumulator = vec2(0);

    ivec2 prevBodyPos = ivec2(0);
    vec2 prevBodyVelocity = vec2(0);

    float oppositeLegWaiting = 0;

    void stopMoving()
    {
        moving = false;
        shouldBeMoving = false;
        distToTargetBeforeMoving = 0;
        timeSinceStartedMoving = 0;
        moveAccumulator = vec2(0);
        oppositeLegWaiting = 0;
    }

END_COMPONENT(Leg)

#endif //GAME_LEG_H
