
#ifndef GAME_ARM_H
#define GAME_ARM_H

#include "../../../../macro_magic/component.h"

/**
 * An arm that can be attached to a body and is procedurally animated.
 *
 * body = e.g. the player entity
 * anchor = the position of the shoulder on the body.
 */
COMPONENT(
    Arm,
    HASH(0),
    FIELD(entt::entity, body),
    FIELD(ivec2, anchor),
    FIELD_DEF_VAL(float, length, 16),
    FIELD_DEF_VAL(float, moveSpeed, 190),

    // if grab is set, then the hand will be attached to the 'grab' entity
    FIELD_DEF_VAL(entt::entity, grab, entt::null)
)

    vec2 moveAccumulator = vec2(0);

END_COMPONENT(Arm)

#endif //GAME_ARM_H
