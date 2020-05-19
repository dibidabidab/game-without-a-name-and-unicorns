
#ifndef GAME_ARROW_H
#define GAME_ARROW_H

#include "../../../../macro_magic/component.h"

COMPONENT(
    Arrow,
    HASH(0),
    FIELD_DEF_VAL(entt::entity, launchedBy, entt::null)
)
END_COMPONENT(Arrow)

#endif //GAME_ARROW_H
