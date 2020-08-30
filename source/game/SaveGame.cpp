
#include "SaveGame.h"
#include "../macro_magic/lua_converters.h"

SaveGame::SaveGame()
{
    luaTable = sol::table::create(luau::getLuaState().lua_state());
}

void to_json(json &j, const SaveGame &s)
{
    j = json::object();
    lua_converter<json>::fromLuaTable(s.luaTable, j["luaTable"]);
}

void from_json(const json &j, SaveGame &s)
{
    lua_converter<json>::toLuaTable(s.luaTable, j.at("luaTable"));
}
