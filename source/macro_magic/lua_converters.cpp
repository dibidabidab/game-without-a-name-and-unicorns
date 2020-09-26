
#include "lua_converters.h"

template<typename luaRef>
void jsonToLua(luaRef &luaVal, const json &json)
{
    if (json.is_structured())
    {
        auto subTable = luaVal.template get_or_create<sol::table>();
        jsonToLuaTable(subTable, json);
    } else if (json.is_number_integer())
        luaVal = int(json);
    else if (json.is_number())
        luaVal = float(json);
    else if (json.is_boolean())
        luaVal = bool(json);
    else if (json.is_string())
        luaVal = std::string(json);
}

void jsonToLuaTable(sol::table &table, const json &json)
{
    assert(json.is_structured());
    if (json.is_object())
        for (auto &[key, val] : json.items())
        {
            auto ref = table[key];
            jsonToLua(ref, val);
        }
    else
        for (auto &[key, val] : json.items())
        {
            auto ref = table[atoi(key.c_str()) + 1];
            jsonToLua(ref, val);
        }
}

void jsonFromLuaTable(const sol::table &table, json &jsonOut)
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
                jsonFromLuaTable(val.as<sol::table>(), jsonVal);
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
