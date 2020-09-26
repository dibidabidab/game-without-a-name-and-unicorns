
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
            jsonFromLuaTable(table, j);
            std::cout << j.dump(indent.has_value() ? indent.value() : -1) << std::endl;
        };

        env["getSettings"] = [] () -> sol::table {
            auto table = sol::table::create(getLuaState().lua_state());
            jsonToLuaTable(table, Game::settings);
            return table;
        };
        env["saveSettings"] = [] (const sol::table &settingsTable) {
            json j;
            jsonFromLuaTable(settingsTable, j);
            Game::settings = j;
        };
        env["getGameStartupArgs"] = [] () -> sol::table {
            auto table = sol::table::create(getLuaState().lua_state());
            jsonToLuaTable(table, Game::startupArgs);
            return table;
        };
        env["tryCloseGame"] = [] {
            gu::setShouldClose(true);
        };

        env["openScreen"] = [] (const char *script) {
            Game::uiScreenManager->openScreen(asset<Script>(script));
        };
        env["closeActiveScreen"] = [] {
            Game::uiScreenManager->closeActiveScreen();
        };

        env["include"] = [&] (const char *scriptPath, const sol::this_environment &currentEnv) -> sol::environment {

            auto newEnv = sol::environment(getLuaState(), sol::create, currentEnv.env.value_or(env));

            asset<Script> toBeIncluded(scriptPath);
            getLuaState().unsafe_script(toBeIncluded->getByteCode().as_string_view(), newEnv);
            return newEnv;
        };

        for (auto &[typeName, info] : SerializableStructInfo::getForAllTypes())
            info->luaUserTypeGenerator(*lua);
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
