
#ifndef GAME_LUAU_H
#define GAME_LUAU_H

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}
#define SOL_ALL_SAFETIES_ON 1

#include <json.hpp>
#include "../external/lua/sol2/single/include/sol/sol.hpp"

namespace luau
{
    void jsonToLuaTable(const json &jsonObj, sol::table &luaTable);
}

#endif //GAME_LUAU_H
