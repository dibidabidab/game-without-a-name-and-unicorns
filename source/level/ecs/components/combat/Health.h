//
// Created by sneeuwpop on 20-05-20.
//

#ifndef GAME_HEALTH_H
#define GAME_HEALTH_H

#include "../../../../macro_magic/component.h"

/**
 * Health of a enemy.
 */
COMPONENT(
        Health,
        HASH(curHealth, maxHealth),
        FIELD_DEF_VAL(int, curHealth, 20),
        FIELD_DEF_VAL(int, maxHealth, 20)
)

END_COMPONENT(Health)

#endif //GAME_HEALTH_H
