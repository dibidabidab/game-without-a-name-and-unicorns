
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
    FIELD_DEF_VAL(float, fireRate, .1),
    FIELD_DEF_VAL(float, distanceFromArcher, 12)
)
    float cooldown = 0;

    entt::entity handBowAnchor;

END_COMPONENT(Bow)

#endif //GAME_BOW_H
