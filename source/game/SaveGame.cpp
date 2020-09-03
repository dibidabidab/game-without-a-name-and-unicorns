
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

const char *SAVE_GAME_ENTITIES_TABLE_NAME = "saveGameEntities";

void SaveGame::save(const char *path)
{
    json j = json::object();
    json &jsonLuaTable = j["luaTable"];
    lua_converter<json>::fromLuaTable(luaTable, jsonLuaTable);

    for (auto &[id, saveData] : jsonLuaTable[SAVE_GAME_ENTITIES_TABLE_NAME].items())
        if (saveData.empty())
            jsonLuaTable[SAVE_GAME_ENTITIES_TABLE_NAME].erase(id);

    std::vector<uint8> data;
    json::to_cbor(j, data);
    File::writeBinary(path == NULL ? loadedFromPath.c_str() : path, data);
}

sol::table SaveGame::getSaveDataForEntity(const std::string &entitySaveGameID, bool temporary)
{
    if (temporary)
        return luau::getLuaState()["tempSaveGameEntities"].get_or_create<sol::table>()[entitySaveGameID].get_or_create<sol::table>();

    return luaTable[SAVE_GAME_ENTITIES_TABLE_NAME].get_or_create<sol::table>()[entitySaveGameID].get_or_create<sol::table>();
}
