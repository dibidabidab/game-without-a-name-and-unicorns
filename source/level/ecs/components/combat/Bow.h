
#ifndef GAME_BOW_H
#define GAME_BOW_H

#include "../../../../macro_magic/component.h"

/**
 * Bow weapon.
 */
COMPONENT(
    Bow,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, archer, entt::null),    // the entity holding the bow.
    FIELD_DEF_VAL(entt::entity, archerLeftArm, entt::null),
    FIELD_DEF_VAL(entt::entity, archerRightArm, entt::null),
    FIELD_DEF_VAL(ivec2, rotatePivot, ivec2(0)),
    FIELD_DEF_VAL(float, fireRate, .2),
    FIELD_DEF_VAL(float, distanceFromArcher, 9)
)
    float cooldown = 0;

    entt::entity handBowAnchor, handStringAnchor;
    vec2 prevAimDir = vec2(1, 0);

END_COMPONENT(Bow)

#endif //GAME_BOW_H
