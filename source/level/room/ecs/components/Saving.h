
#ifndef GAME_SAVING_H
#define GAME_SAVING_H

#include "../../../../macro_magic/component.h"

#include <set>

COMPONENT(
    Persistent,
    HASH(0),
    FIELD(std::string, applyTemplateOnLoad),
    FIELD_DEF_VAL(bool, saveAllComponents, false),
    FIELD(std::set<std::string>, saveComponents)
)
END_COMPONENT(Persistent)

#endif //GAME_SAVING_H
