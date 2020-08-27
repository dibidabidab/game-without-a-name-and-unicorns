
#ifndef GAME_SAVING_H
#define GAME_SAVING_H

#include "../../../macro_magic/component.h"

#include <set>

COMPONENT(
    Persistent,
    HASH(0),
    FIELD(std::string, applyTemplateOnLoad),
    FIELD(std::set<std::string>, excludeComponents)
)
END_COMPONENT(Persistent)

#endif //GAME_SAVING_H
