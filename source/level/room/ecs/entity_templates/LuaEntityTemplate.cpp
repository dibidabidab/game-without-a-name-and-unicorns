
#include "LuaEntityTemplate.h"
#include "../../../../macro_magic/component.h"
#include "../components/LuaScript.h"

LuaEntityTemplate::LuaEntityTemplate(const char *assetName, const char *name, Room *r)
    : script(assetName),
      env(getLuaState(), sol::create, getLuaState().globals())
{
    room = r;

    env["TEMPLATE_NAME"] = name;

    env["defaultArgs"] = [&](const sol::table &table) {
        defaultArgs = table;
    };
    env["description"] = [&](const char *d) {
        description = d;
    };
    env["getTile"] = [&](int x, int y) -> int {
        return int(room->getMap().getTile(x, y));
    };
    env["getTileMaterial"] = [&](int x, int y) -> int {
        return int(room->getMap().getMaterial(x, y));
    };
    env["getLevelTime"] = [&]() -> double {
        return room->getLevel().getTime();
    };
    env["roomWidth"] = room->getMap().width;
    env["roomHeight"] = room->getMap().height;
    env["getComponent"] = [&](int entity, const std::string &componentName) -> sol::optional<sol::table> {

        auto &utils = componentUtils(componentName);

        if (!utils.entityHasComponent(entt::entity(entity), room->entities))
            return sol::optional<sol::table>();

        json j;
        // todo: add utils->componentToLuaTable()
        utils.getJsonComponentWithKeys(j, entt::entity(entity), room->entities);
        auto table = sol::table::create(env.lua_state());
        luau::jsonToLuaTable(j, table);

        return table;
    };
    env["removeComponent"] = [&](int entity, const std::string &componentName) {
        componentUtils(componentName).removeComponent(entt::entity(entity), room->entities);
    };
    env["setComponent"] = [&](int entity, const std::string &componentName, const sol::table &component) {
        luaTableToComponent(entt::entity(entity), componentName, component);
    };
    env["setComponents"] = [&](int entity, const sol::table &componentsTable) {

        for (const auto &[componentName, comp] : componentsTable)
        {
            if (!componentName.is<std::string>())
                throw gu_err("All keys in the components table must be a string!");

            auto nameStr = componentName.as<std::string>();

            if (!comp.is<sol::table>())
                throw gu_err("Expected a table for " + nameStr);
            luaTableToComponent(entt::entity(entity), nameStr, comp);
        }
    };
    env["setUpdateFunction"] = [&](int entity, const sol::function &func, float updateFrequency) {

        LuaScripted &scripted = room->entities.get_or_assign<LuaScripted>(entt::entity(entity));
        scripted.updateFrequency = updateFrequency;
        scripted.updateFunc = func;
        scripted.updateFuncScript = script;
    };

    env["createEntity"] = [&]() -> int {

        return int(room->entities.create());
    };
    env["destroyEntity"] = [&](int e) {

        room->entities.destroy(entt::entity(e));
    };
    env["createChild"] = [&](int parentEntity, sol::optional<std::string> childName) -> int {

        int child = int(createChild(entt::entity(parentEntity), childName.has_value() ? childName.value().c_str() : ""));
        return child;
    };
    env["getChild"] = [&](int parentEntity, const char *childName) -> sol::optional<int> {

        entt::entity childEntity = room->getChildByName(entt::entity(parentEntity), childName);
        if (childEntity == entt::null)
            return sol::optional<int>(); // nil
        return int(childEntity);
    };
    env["applyTemplate"] = [&](int extendE, const char *templateName, sol::optional<sol::table> extendArgs) {

        auto entityTemplate = &room->getTemplate(templateName); // could throw error :)

        if (dynamic_cast<LuaEntityTemplate *>(entityTemplate))
        {
            ((LuaEntityTemplate *) entityTemplate)->
                    createComponentsUsingLuaFunction(entt::entity(extendE), extendArgs);
        }
        else
            entityTemplate->createComponents(entt::entity(extendE));
    };

    // math utils lol:
    env["rotate2d"] = [&](float x, float y, float degrees) -> sol::table {
        auto table = sol::table::create(env.lua_state());

        auto result = rotate(vec2(x, y), degrees * mu::DEGREES_TO_RAD);
        table[1] = result.x;
        table[2] = result.y;
        return table;
    };

    runScript();
}

void LuaEntityTemplate::runScript()
{
    try
    {
        getLuaState().script(script->bytecode.as_string_view(), env);
        createFunc = env["create"];
        onDestroyFunc = env["onDestroy"];
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while running template script " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void LuaEntityTemplate::createComponents(entt::entity e)
{
    createComponentsUsingLuaFunction(e, sol::optional<sol::table>());
}

void LuaEntityTemplate::createComponentsUsingLuaFunction(entt::entity e, sol::optional<sol::table> arguments)
{
    if (script.hasReloaded())
        runScript();

    try
    {
        if (arguments.has_value() && !defaultArgs.empty())
        {
            for (auto &[key, defaultVal] : defaultArgs)
            {
                if (!arguments.value()[key].valid())
                    arguments.value()[key] = defaultVal;
            }
        }

        sol::protected_function_result result = createFunc(e, arguments);
        if (!result.valid())
        {
            sol::error err = result.get<sol::error>();
            std::string what = err.what();
            throw gu_err(sol::error(result).what());
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while creating entity using " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

sol::state &LuaEntityTemplate::getLuaState()
{
    static sol::state *lua = NULL;

    if (lua == NULL)
    {
        lua = new sol::state;
        lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
    }
    return *lua;
}

const std::string &LuaEntityTemplate::getDescription()
{
    if (script.hasReloaded())
        runScript();
    return description;
}

json LuaEntityTemplate::getDefaultArgs()
{
    if (script.hasReloaded())
        runScript();
    json j;
    lua_converter<json>::getFrom(defaultArgs, j);
    return j;
}

void LuaEntityTemplate::createComponentsUsingLuaFunction(entt::entity e, const json &arguments)
{
    auto table = sol::table::create(env.lua_state());
    luau::jsonToLuaTable(arguments, table);
    createComponentsUsingLuaFunction(e, table);
}

void LuaEntityTemplate::luaTableToComponent(entt::entity e, const std::string &componentName, const sol::table &component)
{
    componentUtils(componentName).setFromLuaTable(component, e, room->entities);
}

const ComponentUtils &LuaEntityTemplate::componentUtils(const std::string &componentName)
{
    auto utils = ComponentUtils::getFor(componentName);
    if (!utils)
        throw gu_err("Component-type named '" + componentName + "' does not exist!");
    return *utils;
}


LuaEntityScript::LuaEntityScript(std::string s) : source(std::move(s))
{
    sol::load_result lr = LuaEntityTemplate::getLuaState().load(source);
    if (!lr.valid())
        throw gu_err("Lua code invalid!");
    bytecode = sol::protected_function(lr).dump();
}
