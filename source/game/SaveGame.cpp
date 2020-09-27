
#include "SaveGame.h"
#include "../macro_magic/lua_converters.h"
#include "Game.h"

SaveGame::SaveGame(const char *path) : loadedFromPath(path)
{
    luaTable = sol::table::create(luau::getLuaState().lua_state());

    if (File::exists(path))
    {
        auto data = File::readBinary(path);
        json jsonData = json::from_cbor(data.begin(), data.end());
        jsonToLuaTable(luaTable, jsonData.at("luaTable"));
    }
}

const char *SAVE_GAME_ENTITIES_TABLE_NAME = "saveGameEntities";

void SaveGame::save(const char *path)
{
    json j = json::object();
    json &jsonLuaTable = j["luaTable"] = json::object();
    jsonFromLuaTable(luaTable, jsonLuaTable);

    std::vector<std::string> idsToRemove;

    for (auto &[id, saveData] : jsonLuaTable[SAVE_GAME_ENTITIES_TABLE_NAME].items())
        if (saveData.empty())
            idsToRemove.push_back(id);

    for (auto &id : idsToRemove)
        jsonLuaTable[SAVE_GAME_ENTITIES_TABLE_NAME].erase(id);

    std::vector<uint8> data;
    json::to_cbor(j, data);
    File::writeBinary(path == NULL ? loadedFromPath.c_str() : path, data);
}

sol::table SaveGame::getSaveDataForEntity(const std::string &entitySaveGameID, bool temporary)
{
    if (temporary)
        return luau::getLuaState()["tempSaveGameEntities"].get_or_create<sol::table>()[entitySaveGameID].get_or_create<sol::table>();

    auto &saveGameLuaTable = Game::tryGetCurrentSession()->saveGame.luaTable;

    return saveGameLuaTable[SAVE_GAME_ENTITIES_TABLE_NAME].get_or_create<sol::table>()[entitySaveGameID].get_or_create<sol::table>();
}
