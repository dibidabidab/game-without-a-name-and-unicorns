
#ifndef GAME_BOW_H
#define GAME_BOW_H

#include "../../../../../macro_magic/component.h"

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

    FIELD_DEF_VAL(entt::entity, handBowAnchor, entt::null),
    FIELD_DEF_VAL(entt::entity, handStringAnchor, entt::null),

    FIELD_DEF_VAL(float, fireRate, .4),
    FIELD_DEF_VAL(float, distanceFromArcher, 9),

    FIELD_DEF_VAL(std::string, arrowTemplate, "Arrow")
)
    float cooldown = 0;

    vec2 prevAimDir = vec2(1, 0);

END_COMPONENT(Bow)

#endif //GAME_BOW_H
