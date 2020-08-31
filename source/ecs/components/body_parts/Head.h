
#ifndef GAME_HEAD_H
#define GAME_HEAD_H

#include "../../../macro_magic/component.h"

COMPONENT(
    Head,
    HASH(0),
    FIELD(entt::entity, body),
    FIELD_DEF_VAL(int, maxHeadRetraction, 4),
    FIELD_DEF_VAL(float, turnHeadDelay, .05)
)

    float timeSinceBodyFlipped = 0.;

END_COMPONENT(Head)

#endif //GAME_HEAD_H
