
#ifndef GAME_UICOMPONENTS_H
#define GAME_UICOMPONENTS_H

#include <utils/aseprite/Aseprite.h>
#include "../../../macro_magic/component.h"

COMPONENT(
    NineSlicePlane,
    HASH(0),
    FIELD(asset<aseprite::Sprite>, sprite)
)
END_COMPONENT(NineSlicePlane)


COMPONENT(
    TextView,
    HASH(0),
    FIELD(std::string, text),
    FIELD_DEF_VAL(bool, wrapInAABB, false)
)
END_COMPONENT(TextView)


#endif //GAME_UICOMPONENTS_H
