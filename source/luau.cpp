
#include "luau.h"
#include "game/Game.h"

struct Test
{
    int poepie = 4;
    ivec2 point = ivec2(10);
    asset<aseprite::Sprite> sprite;
};

struct TestVec
{
    std::vector<Test> tests;
};


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
        env["getGameStartupArgs"] = [] () -> sol::table {
            auto table = sol::table::create(getLuaState().lua_state());
            lua_converter<json>::toLuaTable(table, Game::startupArgs);
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

        sol::usertype<Test> testType = lua->new_usertype<Test>("Test");
        testType["poepie"] = &Test::poepie;
        testType["point"] = &Test::point;
        testType["sprite"] = &Test::sprite;

        sol::usertype<TestVec> testVecType = lua->new_usertype<TestVec>("TestVec");
        testVecType["tests"] = &TestVec::tests;


        TestVec vec;
        vec.tests.emplace_back();

        env["vec"] = &vec;

        assert(env["vec"].get_type() == sol::type::userdata);

        std::cout << env["vec"]["__type"]["name"].get<std::string>() << '\n';

        std::cout << int(env["vec"].get_type()) << '\n';

        lua->unsafe_script("vec.tests[1].poepie = 12345");

        assert(vec.tests[0].poepie == 12345);

        lua->unsafe_script("vec.tests[2] = Test.new()");
        lua->unsafe_script("vec.tests[2].poepie = -3");

        assert(vec.tests[1].poepie == -3);

        Test lol;

        env["lol"] = &lol;

        lua->unsafe_script("lol.poepie = 12");
        lua->unsafe_script("lol.sprite = 'sprites/enemy'");

        assert(lol.poepie == 12);
        std::cout << lol.sprite.getLoadedAsset().fullPath << '\n';

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
