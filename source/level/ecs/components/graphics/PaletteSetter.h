
#ifndef GAME_PALETTESETTER_H
#define GAME_PALETTESETTER_H

#include "../../../../macro_magic/component.h"

COMPONENT(
    PaletteSetter,
    HASH(0),
    FIELD_DEF_VAL(int, priority, 0),
    FIELD(std::string, paletteName)
)
END_COMPONENT(PaletteSetter)

#endif
