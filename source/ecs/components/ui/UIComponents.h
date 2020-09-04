
#ifndef GAME_UICOMPONENTS_H
#define GAME_UICOMPONENTS_H

#include <utils/aseprite/Aseprite.h>
#include "../../../macro_magic/component.h"

COMPONENT(
    UIElement,
    HASH(0),
    FIELD_DEF_VAL(ivec2, topLeftPos, ivec2(0)),
    FIELD_DEF_VAL(ivec2, size, ivec2(4))
)
END_COMPONENT(UIElement)



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
    FIELD(asset<aseprite::Sprite>, fontSprite),
    FIELD_DEF_VAL(uint8, mapColorFrom, 0u),
    FIELD_DEF_VAL(uint8, mapColorTo, 0u),
    FIELD_DEF_VAL(int, lineSpacing, 3),
    FIELD_DEF_VAL(int, letterSpacing, 1),

    FIELD_DEF_VAL(bool, waving, false),
    FIELD_DEF_VAL(float, wavingFrequency, 1.),
    FIELD_DEF_VAL(float, wavingSpeed, 1.),
    FIELD_DEF_VAL(float, wavingAmplitude, 8.)
)
END_COMPONENT(TextView)


#endif //GAME_UICOMPONENTS_H
