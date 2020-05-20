//
// Created by sneeuwpop on 20-05-20.
//

#ifndef GAME_KNOCKBACK_H
#define GAME_KNOCKBACK_H

#include "../../../../macro_magic/component.h"

/**
 * Health of a enemy.
 */
COMPONENT(
        KnockBack,
        HASH(knockBackForce),
        FIELD_DEF_VAL(float, knockBackForce, 0),
        FIELD_DEF_VAL(vec2, knockBackDirection, vec2(0))
)

END_COMPONENT(KnockBack)

#endif //GAME_KNOCKBACK_H

