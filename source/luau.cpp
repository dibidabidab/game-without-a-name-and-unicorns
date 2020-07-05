
#include "luau.h"

template<typename ref>
void jsonValToRef(const json &val, ref &r)
{
    if (val.is_structured())
    {
        auto subTable = r.template get_or_create<sol::table>();
        luau::jsonToLuaTable(val, subTable);
    }
    else if (val.is_number_integer())
        r = int(val);
    else if (val.is_number())
        r = float(val);
    else if (val.is_boolean())
        r = bool(val);
    else if (val.is_string())
        r = std::string(val);
}

void luau::jsonToLuaTable(const json &jsonObj, sol::table &luaTable)
{
    assert(jsonObj.is_structured());

    for (auto &[key, val] : jsonObj.items())
    {
        if (jsonObj.is_object())
        {
            auto ref = luaTable[key];
            jsonValToRef(val, ref);
        }
        else
        {
            auto ref = luaTable[atoi(key.c_str()) + 1];
            jsonValToRef(val, ref);
        }
    }
}
