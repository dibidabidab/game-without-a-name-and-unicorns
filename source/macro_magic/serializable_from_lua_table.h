
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
    static void getFrom(sol::object v, type &field)
    {
        if (!v.valid())
            return;

        auto optional = v.as<sol::optional<type>>();
        if (optional.has_value())
            field = optional.value();
        else
        {
            auto info = SerializableStructInfo::getFor<type>();

            if (!info)
                throw gu_err("Unable to convert to " + getTypeName<type>());

            auto optionalTable = v.as<sol::optional<sol::table>>();
            if (!optionalTable.has_value())
                throw gu_err("Unable to convert to " + getTypeName<type>() + " as it is not a table!");

            info->fromLuaTableFunction(&field, optionalTable.value());
        }
    }
};

template <int len, typename type, qualifier something>
struct lua_converter<vec<len, type, something>>
{
    static void getFrom(sol::object v, vec<len, type, something> &vec)
    {
        if (!v.valid())
            return;

        auto vecTable = v.as<sol::optional<sol::table>>();

        if (!vecTable.has_value() || vecTable.value().size() != len)
            throw gu_err("In order to convert to vec" + std::to_string(len) + ", value should be a table with " + std::to_string(len) + " numbers!");

        for (int i = 0; i < len; i++)
            vec[i] = vecTable.value()[i + 1].get<float>(); // get<float>() because get<int>() might cause error when value is a float
    }
};

template <typename type>
struct lua_converter<asset<type>>
{
    static void getFrom(sol::object v, asset<type> &asset)
    {
        if (!v.valid())
            return;

        auto path = v.as<sol::optional<std::string>>();
        if (!path.has_value())
            throw gu_err("In order to set asset<" + getTypeName<type>() + ">, value should be a string (example: 'sprites/my_very_nice_sprite').");

        asset.set(path.value());
    }
};

template <typename type>
struct lua_converter<std::vector<type>>
{
    static void getFrom(sol::object v, std::vector<type> &vec)
    {
        if (!v.valid())
            return;

        auto listTable = v.as<sol::optional<sol::table>>();

        if (!listTable.has_value())
            throw gu_err("Cannot convert non-table to vector :(");

        vec.resize(listTable.value().size());

        int i = 0;
        for (auto &[_, val] : listTable.value())
            lua_converter<type>::getFrom(val, vec[i++]);
    }
};

template <typename type>
struct lua_converter<std::list<type>>
{
    static void getFrom(sol::object v, std::list<type> &list)
    {
        if (!v.valid())
            return;

        auto listTable = v.as<sol::optional<sol::table>>();

        if (!listTable.has_value())
            throw gu_err("Cannot convert non-table to list :(");

        list.clear();

        for (auto &[key, val] : listTable.value())
        {
            list.emplace_back();
            type &back = list.back();
            lua_converter<type>::getFrom(val, back);
        }
    }
};


#define PULL_FIELD_OUT_LUA_TABLE(field) \
    __PULL_FIELD_OUT_LUA_TABLE__(EAT field)

#define __PULL_FIELD_OUT_LUA_TABLE__(X)  \
    lua_converter<ARGTYPE(X)>::getFrom(table[ARGNAME_AS_STRING(X)], ARGNAME(X))


// the macro to be used in serializable.h:
#define FROM_LUA_TABLE(...)\
    DOFOREACH_SEMICOLON(PULL_FIELD_OUT_LUA_TABLE, __VA_ARGS__)

#endif //GAME_SERIALIZABLE_FROM_LUA_TABLE_H
