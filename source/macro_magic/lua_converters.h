
#ifndef GAME_LUA_CONVERTERS_H
#define GAME_LUA_CONVERTERS_H

#include "../luau.h"
#include "macro_helpers.h"
#include "sfinae.h"

#include <iostream>
#include <utils/gu_error.h>
#include <utils/type_name.h>
#include <utils/math_utils.h>
#include <asset_manager/asset.h>

HAS_MEM_FUNC(toLuaTable, has_toLuaTable_function)
HAS_MEM_FUNC(fromLuaTable, has_fromLuaTable_function)

template<typename type>
struct lua_converter
{
    static void fromLua(sol::object v, type &field)
    {
        if (!v.valid())
            return;

        if constexpr (has_fromLuaTable_function<type>::value)
        {
            auto optionalTable = v.as<sol::optional<sol::table>>();
            if (!optionalTable.has_value())
                throw gu_err("Unable to convert to " + getTypeName<type>() + " because the given Lua value is not a table!");

            field.fromLuaTable(optionalTable.value());
        }
        else
        {
            auto optional = v.as<sol::optional<type>>();
            if (optional.has_value())
                field = optional.value();
            else
                throw gu_err("Unable to convert to " + getTypeName<type>());

            /**
             * if this ever fails then this might work:
             *
             * if constexpr (has_fromJson_function<type>::value)
             * {
             *      convert lua value to json, then call fromJson<type>(json, type())
             * }
             */
        }
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const type &val)
    {
        if constexpr (has_toLuaTable_function<type>::value)
        {
            auto table = luaVal.template get_or_create<sol::table>();
            val.toLuaTable(table);
        }
        else
            luaVal = val;
    }
};


template<>
struct lua_converter<uint8>
{
    static void fromLua(sol::object v, uint8 &c)
    {
        if (!v.valid())
            return;
        c = v.as<sol::optional<uint8>>().value_or(v.as<sol::optional<float>>().value_or(-123));
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const uint8 &c)
    {
        luaVal = c;
    }
};

template<int len, typename type, qualifier something>
struct lua_converter<vec<len, type, something>>
{
    static void fromLua(sol::object v, vec<len, type, something> &vec)
    {
        if (!v.valid())
            return;

        auto vecTable = v.as<sol::optional<sol::table>>();

        if (!vecTable.has_value() || vecTable.value().size() != len)
            throw gu_err("In order to convert to vec" + std::to_string(len) + ", value should be a table with " +
                         std::to_string(len) + " numbers!");

        for (int i = 0; i < len; i++)
            vec[i] = vecTable.value()[i + 1].get<float>(); // get<float>() because get<int>() might cause error when value is a float
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const vec<len, type, something> &vec)
    {
        auto vecTable = luaVal.template get_or_create<sol::table>();
        for (int i = 0; i < len; i++)
            vecTable[i + 1] = vec[i];
    }
};

template<typename type>
struct lua_converter<asset<type>>
{
    static void fromLua(sol::object v, asset<type> &asset)
    {
        if (!v.valid())
            return;

        auto path = v.as<sol::optional<std::string>>();
        if (!path.has_value())
            throw gu_err("In order to set asset<" + getTypeName<type>() +
                         ">, value should be a string (example: 'sprites/my_very_nice_sprite').");

        asset.set(path.value());
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const asset<type> &asset)
    {
        if (asset.isSet())
            luaVal = asset.getLoadedAsset().shortPath;
        else
            luaVal = "";
    }
};

template<typename type>
struct lua_converter<std::vector<type>>
{
    static void fromLua(sol::object v, std::vector<type> &vec)
    {
        if (!v.valid())
            return;

        auto listTable = v.as<sol::optional<sol::table>>();

        if (!listTable.has_value())
            throw gu_err("Cannot convert non-table to vector :(");

        vec.resize(listTable.value().size());

        int i = 0;
        for (auto &[_, val] : listTable.value())
            lua_converter<type>::fromLua(val, vec[i++]);
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const std::vector<type> &vec)
    {
        int i = 1;
        auto table = luaVal.template get_or_create<sol::table>();
        for (const type &v : vec)
        {
            auto ref = table[i++];
            lua_converter<type>::toLua(ref, v);
        }
    }
};

template<typename type>
struct lua_converter<std::list<type>>
{
    static void fromLua(sol::object v, std::list<type> &list)
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
            lua_converter<type>::fromLua(val, back);
        }
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const std::list<type> &list)
    {
        int i = 1;
        auto table = luaVal.template get_or_create<sol::table>();
        for (const type &v : list)
        {
            auto ref = table[i++];
            lua_converter<type>::toLua(ref, v);
        }
    }
};

template<>
struct lua_converter<json>
{
    static void fromLuaTable(const sol::table &table, json &jsonOut)
    {
        jsonOut = json::object(); // assume its an object (keys are strings).
        for (auto &[key, val] : table)
        {
            if (jsonOut.is_object() && key.get_type() == sol::type::number)
                jsonOut = json::array();

            json jsonVal;

            switch (val.get_type())
            {
                case sol::type::number:
                    jsonVal = val.as<double>();
                    break;
                case sol::type::boolean:
                    jsonVal = val.as<bool>();
                    break;
                case sol::type::string:
                    jsonVal = val.as<std::string>();
                    break;
                case sol::type::table:
                    fromLuaTable(val.as<sol::table>(), jsonVal);
                    break;
                default:
                    break;
            }
            if (jsonOut.is_object())
                jsonOut[key.as<std::string>()] = jsonVal;
            else
                jsonOut.push_back(jsonVal);
        }
    }

    static void fromLua(sol::object v, json &jsonOut)
    {
        if (!v.valid())
            return;

        auto listTable = v.as<sol::optional<sol::table>>();
        if (listTable.has_value())
            fromLuaTable(listTable.value(), jsonOut);
    }

    static void toLuaTable(sol::table &table, const json &json)
    {
        assert(json.is_structured());
        if (json.is_object())
            for (auto &[key, val] : json.items())
            {
                auto ref = table[key];
                toLua(ref, val);
            }
        else
            for (auto &[key, val] : json.items())
            {
                auto ref = table[atoi(key.c_str()) + 1];
                toLua(ref, val);
            }
    }

    template<typename luaRef>
    static void toLua(luaRef &luaVal, const json &json)
    {
        if (json.is_structured())
        {
            auto subTable = luaVal.template get_or_create<sol::table>();
            toLuaTable(subTable, json);
        } else if (json.is_number_integer())
            luaVal = int(json);
        else if (json.is_number())
            luaVal = float(json);
        else if (json.is_boolean())
            luaVal = bool(json);
        else if (json.is_string())
            luaVal = std::string(json);
    }
};


// MACROS FOR Lua->Struct

#define PULL_FIELD_OUT_LUA_TABLE(field) \
    __PULL_FIELD_OUT_LUA_TABLE__(EAT field)

#define __PULL_FIELD_OUT_LUA_TABLE__(X)  \
    lua_converter<ARGTYPE(X)>::fromLua(table[ARGNAME_AS_STRING(X)], ARGNAME(X))


// the macro to be used in serializable.h:
#define FROM_LUA_TABLE(...)\
    DOFOREACH_SEMICOLON(PULL_FIELD_OUT_LUA_TABLE, __VA_ARGS__)

// END MACROS FOR Lua->Struct




// MACROS FOR Struct->Lua

#define PUT_FIELD_IN_LUA_TABLE(field) \
    __PUT_FIELD_IN_LUA_TABLE__(EAT field)

#define __PUT_FIELD_IN_LUA_TABLE__(X) \
    {\
        auto ref = table[ARGNAME_AS_STRING(X)];\
        lua_converter<ARGTYPE(X)>::toLua(ref, ARGNAME(X));\
    }


// the other macro to be used in serializable.h:
#define TO_LUA_TABLE(...)\
    DOFOREACH_SEMICOLON(PUT_FIELD_IN_LUA_TABLE, __VA_ARGS__)


// END MACROS FOR Struct->Lua

#endif //GAME_LUA_CONVERTERS_H
