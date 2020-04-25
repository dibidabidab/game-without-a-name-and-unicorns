
#ifndef GAME_LIGHT_H
#define GAME_LIGHT_H

#include "../../../macro_magic/component.h"

COMPONENT(
    LightPoint,
    HASH(radius, castShadow),

    FIELD_DEF_VAL(uint8, radius, 128),
    FIELD_DEF_VAL(bool, castShadow, true)
)
END_COMPONENT(LightPoint)

#endif //GAME_LIGHT_H
