
#ifndef GAME_SERIALIZABLE_FROM_LUA_TABLE_H
#define GAME_SERIALIZABLE_FROM_LUA_TABLE_H

#include "../luau.h"
#include "macro_helpers.h"

#include <iostream>
#include <utils/gu_error.h>
#include <utils/type_name.h>
#include <utils/math_utils.h>
#include <asset_manager/asset.h>


template<typename type>
struct lua_converter
{
    static void getFrom(type &field, const sol::table &table, const char *key)
    {
        sol::table_proxy v = table[key];
        if (!v.valid())
            return;

        auto optional = v.get<sol::optional<type>>();
        if (optional.has_value())
            field = optional.value();
        else
        {
            auto info = SerializableStructInfo::getFor(getTypeName<type>().c_str());

            if (!info)
                throw gu_err("Unable to convert '" + std::string(key) + "' to " + getTypeName<type>());

            auto optionalTable = v.get<sol::optional<sol::table>>();
            if (!optionalTable.has_value())
                throw gu_err("Unable to convert '" + std::string(key) + "' to " + getTypeName<type>() + " as it is not a table!");

            info->fromLuaTableFunction(&field, optionalTable.value());
        }
    }
};

template <int len, typename type, qualifier something>
struct lua_converter<vec<len, type, something>>
{
    static void getFrom(vec<len, type, something> &v, const sol::table &table, const char *key)
    {
        auto val = table[key];

        if (!val.valid())
            return;

        sol::optional<sol::table> vecTable = val;
        if (!vecTable.has_value())
            throw gu_err("In order to convert to vec" + std::to_string(len) + ", '" + std::string(key) + "' should be a table with " + std::to_string(len) + " numbers!");

        for (int i = 0; i < len; i++)
            v[i] = vecTable.value()[i + 1].get<type>();
    }
};

template <typename type>
struct lua_converter<asset<type>>
{
    static void getFrom(asset<type> &asset, const sol::table &table, const char *key)
    {
        auto val = table[key];

        if (!val.valid())
            return;

        sol::optional<std::string> path = val;
        if (!path.has_value())
            throw gu_err("In order to set asset<" + getTypeName<type>() + ">, '" + std::string(key) + "' should be a string (example: 'sprites/my_very_nice_sprite').");

        asset.set(path.value());
    }
};

#define PULL_FIELD_OUT_LUA_TABLE(field) \
    __PULL_FIELD_OUT_LUA_TABLE__(EAT field)

#define __PULL_FIELD_OUT_LUA_TABLE__(X)  \
    lua_converter<typeof(ARGNAME(X))>::getFrom(ARGNAME(X), table, ARGNAME_AS_STRING(X))


// the macro to be used in serializable.h:
#define FROM_LUA_TABLE(...)\
    DOFOREACH_SEMICOLON(PULL_FIELD_OUT_LUA_TABLE, __VA_ARGS__)

#endif //GAME_SERIALIZABLE_FROM_LUA_TABLE_H
