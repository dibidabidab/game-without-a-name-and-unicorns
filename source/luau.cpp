
#include "luau.h"
#include "game/Game.h"

sol::state &luau::getLuaState()
{
    static sol::state *lua = NULL;

    if (lua == NULL)
    {
        lua = new sol::state;
        lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);

        auto &env = lua->globals();

        // math utils lol:
        env["rotate2d"] = [&](float x, float y, float degrees) -> sol::table {
            auto table = sol::table::create(env.lua_state());

            auto result = rotate(vec2(x, y), degrees * mu::DEGREES_TO_RAD);
            table[1] = result.x;
            table[2] = result.y;
            return table;
        };

        // colors:
        auto colorTable = sol::table::create(env.lua_state());
        Palette &palette = Game::palettes->effects.at(0).lightLevels[0].get();
        int i = 0;
        for (auto &[name, color] : palette.colors)
            colorTable[name] = i++;
        env["colors"] = colorTable;

        env["printTableAsJson"] = [&] (const sol::table &table, sol::optional<int> indent) // todo: this doest work properly
        {
            json j;
            lua_converter<json>::fromLua(table, j);
            std::cout << j.dump(indent.has_value() ? indent.value() : -1) << std::endl;
        };

        env["getSaveGame"] = [] () -> sol::table {
            return Game::getSaveGame().luaTable;
        };
        env["tryGetSaveGame"] = [] () -> sol::optional<sol::table> {
            auto sg = Game::tryGetSaveGame();
            if (sg)
                return sg->luaTable;
            else return sol::optional<sol::table>();
        };
        env["saveSaveGame"] = [] (sol::optional<std::string> path) { // no path means the path the saveGame was loaded from
            Game::saveSaveGame(path.has_value() ? path.value().c_str() : NULL);
        };
        env["loadOrCreateSaveGame"] = [] (const char *path) {
            Game::loadOrCreateSaveGame(path);
        };

        env["getSettings"] = [] () -> sol::table {
            auto table = sol::table::create(getLuaState().lua_state());
            lua_converter<json>::toLuaTable(table, Game::settings);
            return table;
        };
        env["saveSettings"] = [] (const sol::table &settingsTable) {
            json j;
            lua_converter<json>::fromLuaTable(settingsTable, j);
            Game::settings = j;
        };
    }
    return *lua;
}

luau::Script::Script(const std::string &path) : path(path)
{}

const sol::bytecode &luau::Script::getByteCode()
{
    if (!bytecode.empty())
        return bytecode;

    sol::load_result lr = luau::getLuaState().load_file(path);
    if (!lr.valid())
        throw gu_err("Lua code invalid!");

    bytecode = sol::protected_function(lr).dump();
    return bytecode;
}
