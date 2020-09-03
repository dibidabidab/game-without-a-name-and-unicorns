
#ifndef GAME_LUASCRIPTED_H
#define GAME_LUASCRIPTED_H

#include "../../macro_magic/component.h"
#include "../entity_templates/LuaEntityTemplate.h"

COMPONENT(
    LuaScripted,
    HASH(0),
    FIELD_DEF_VAL(float, updateFrequency, 1.)
)

    float updateAccumulator = 0.;

    sol::function updateFunc, onDestroyFunc;
    asset<luau::Script> updateFuncScript, onDestroyFuncScript;

    sol::table saveData;

END_COMPONENT(LuaScripted)


#endif //GAME_LUASCRIPTED_H
