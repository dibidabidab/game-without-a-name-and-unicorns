
#ifndef GAME_LUA_CONVERTERS_H
#define GAME_LUA_CONVERTERS_H

#include "../luau.h"
#include "../../external/entt/src/entt/entity/registry.hpp"

#include <asset_manager/asset.h>

/////////// asset<>

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
///////////

/////////// entt::entity

template <typename Handler>
bool sol_lua_check(sol::types<entt::entity>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking) {

    bool success = sol::stack::check<int>(L, index, handler);
    if (!success)
        success = sol::stack::check<sol::nil_t>(L, index, handler);
    tracking.use(1);
    return success;
}

entt::entity sol_lua_get(sol::types<entt::entity>, lua_State* L, int index, sol::stack::record& tracking);

int sol_lua_push(sol::types<entt::entity>, lua_State* L, const entt::entity& e);

///////////////

void jsonFromLuaTable(const sol::table &table, json &jsonOut);

void jsonToLuaTable(sol::table &table, const json &json);

template <>
struct sol::usertype_container<json> : public container_detail::usertype_container_default<json>
{
    static int index_get(lua_State *lua)
    {
        json &j = *sol::stack::unqualified_check_get<json *>(lua, 1).value();

        json *jOut = NULL;

        const char *keyStr = sol::stack::unqualified_check_get<const char *>(lua, 2).value_or((const char *) NULL);
        if (keyStr)
            jOut = &j.at(keyStr);

        else
        {
            int keyInt = sol::stack::unqualified_check_get<int>(lua, 2).value();
            jOut = &j.at(keyInt);
        }
        if (jOut->is_structured())
            sol::stack::push(lua, jOut);
        else if (jOut->is_boolean())
            sol::stack::push(lua, bool(*jOut));
        else if (jOut->is_null())
            sol::stack::push(lua, sol::nil);
        else if (jOut->is_number_float())
            sol::stack::push(lua, float(*jOut));
        else if (jOut->is_number())
            sol::stack::push(lua, int(*jOut));
        else if (jOut->is_string())
            sol::stack::push(lua, std::string(*jOut));
        return 1;
    }

};

#endif //GAME_LUA_CONVERTERS_H
