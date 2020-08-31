
#ifndef GAME_SAVING_H
#define GAME_SAVING_H

#include "../../macro_magic/component.h"

COMPONENT(
    Persistent,
    HASH(0),
    FIELD(std::string, applyTemplateOnLoad),
    FIELD(json, data),

    FIELD_DEF_VAL(bool, saveFinalPosition, false),
    FIELD_DEF_VAL(bool, saveSpawnPosition, false),
    FIELD_DEF_VAL(ivec2, spawnPosition, ivec2(0)),

    FIELD_DEF_VAL(bool, revive, false),
    FIELD_DEF_VAL(bool, saveAllComponents, false),
    FIELD(std::vector<std::string>, saveComponents)
)
END_COMPONENT(Persistent)

#endif //GAME_SAVING_H
