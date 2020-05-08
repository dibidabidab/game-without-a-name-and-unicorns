
#ifndef GAME_DRAWPOLYLINE_H
#define GAME_DRAWPOLYLINE_H

#include "../../../macro_magic/component.h"

COMPONENT(
    DrawPolyline,
    HASH(0),
    FIELD(std::vector<uint8>, colors),
    FIELD_DEF_VAL(float, zIndexBegin, 0),
    FIELD_DEF_VAL(float, zIndexEnd, 0)
)
END_COMPONENT(DrawPolyline)

#endif //GAME_DRAWPOLYLINE_H
