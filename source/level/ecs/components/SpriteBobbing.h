
#ifndef GAME_SPRITEBOBBING_H
#define GAME_SPRITEBOBBING_H

#include "../../../macro_magic/component.h"

/**
 * Makes an entity's sprite move up and down based on the position of the entity's feet
 */
COMPONENT(
    SpriteBobbing,
    HASH(0),
    FIELD(std::vector<entt::entity>, feet)
)
END_COMPONENT(SpriteBobbing)

#endif //GAME_SPRITEBOBBING_H
