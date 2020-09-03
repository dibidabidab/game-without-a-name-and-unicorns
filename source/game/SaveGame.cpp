
#include "SaveGame.h"
#include "../macro_magic/lua_converters.h"

SaveGame::SaveGame(const char *path) : loadedFromPath(path)
{
    luaTable = sol::table::create(luau::getLuaState().lua_state());

    if (File::exists(path))
    {
        auto data = File::readBinary(path);
        json jsonData = json::from_cbor(data.begin(), data.end());
        lua_converter<json>::toLuaTable(luaTable, jsonData.at("luaTable"));
    }
}

void SaveGame::save(const char *path)
{
    json j = json::object();
    lua_converter<json>::fromLuaTable(luaTable, j["luaTable"]);
    std::vector<uint8> data;
    json::to_cbor(j, data);
    File::writeBinary(path == NULL ? loadedFromPath.c_str() : path, data);
}
