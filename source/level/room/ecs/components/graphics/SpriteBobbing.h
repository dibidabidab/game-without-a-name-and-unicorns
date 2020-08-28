
#ifndef GAME_SPRITEBOBBING_H
#define GAME_SPRITEBOBBING_H

#include "../../../../../macro_magic/component.h"

/**
 * Makes an entity's sprite move up and down based on the position of the entity's feet
 */
COMPONENT(
    SpriteBobbing,
    HASH(0),
    FIELD(std::vector<entt::entity>, feet),
    FIELD_DEF_VAL(int, maxYPos, 10),
    FIELD_DEF_VAL(int, minYPos, -6)
)

    float floorHitVelocity = 0;
    float floorHitYPos = 0;

END_COMPONENT(SpriteBobbing)

#endif //GAME_SPRITEBOBBING_H
