
#ifndef GAME_LUA_CONVERTERS_H
#define GAME_LUA_CONVERTERS_H

#include "../luau.h"

#include <asset_manager/asset.h>

//template<int len, typename type, qualifier something>


template <typename Handler, typename type>
bool sol_lua_check(sol::types<asset<type>>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking) {

    bool success = sol::stack::check<const char *>(L, index, handler);
    tracking.use(1);
    return success;
}

template<typename type>
asset<type> sol_lua_get(sol::types<asset<type>>, lua_State* L, int index, sol::stack::record& tracking)
{
    sol::optional<const char *> path = sol::stack::get<sol::optional<const char *>>(L, index);

    tracking.use(1);

    asset<type> a;

    if (path.has_value())
        a.set(path.value());

    return a;
}

template<typename type>
int sol_lua_push(sol::types<asset<type>>, lua_State* L, const asset<type>& asset) {

    if (asset.isSet())
        sol::stack::push(L, asset.getLoadedAsset()->shortPath);
    else
        sol::stack::push(L, sol::nil);

    return 1;
}

// TODO: SOL FUNCTIONS FOR entt::entity, (nil<->entity::null), and replace lambda int args with entt::entity!!!!!!

void jsonFromLuaTable(const sol::table &table, json &jsonOut);

void jsonToLuaTable(sol::table &table, const json &json);


#endif //GAME_LUA_CONVERTERS_H
