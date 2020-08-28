
#include "LuaEntityTemplate.h"
#include "../../../../macro_magic/component.h"

LuaEntityTemplate::LuaEntityTemplate(const char *assetName, const char *name, Room *r)
    : script(assetName),
      env(getLuaState(), sol::create, getLuaState().globals())
{
    room = r;

    env["TEMPLATE_NAME"] = name;

    env["arg"] = [&](const char *argName, sol::optional<sol::reference> defaultVal) {
        if (!env["args"][argName].valid())
            env["args"][argName] = defaultVal;
    };
    env["getTile"] = [&](int x, int y) -> int {
        return static_cast<int>(room->getMap().getTile(x, y));
    };
    env["getTileMaterial"] = [&](int x, int y) -> int {
        return static_cast<int>(room->getMap().getMaterial(x, y));
    };
    env["getLevelTime"] = [&]() -> double {
        return room->getLevel().getTime();
    };
    env["roomWidth"] = room->getMap().width;
    env["roomHeight"] = room->getMap().height;
    env["getComponent"] = [&](int fromEntity, const std::string &componentName) -> sol::optional<sol::table> {

        auto utils = ComponentUtils::getFor(componentName);
        if (!utils)
            throw gu_err("Error while calling getComponent(): Component Type named " + componentName + " does NOT exist!");

        if (!utils->entityHasComponent(static_cast<entt::entity>(fromEntity), room->entities))
            return sol::optional<sol::table>();

        json j;
        utils->getJsonComponentWithKeys(j, static_cast<entt::entity>(fromEntity), room->entities);
        auto table = sol::table::create(env.lua_state());
        luau::jsonToLuaTable(j, table);

        return table;
    };

    env["createChild"] = [&](const char *childName) -> int {

//        if (env[childName].valid())
//            throw gu_err("Could not create child.\nIt seems that the lua script already has a variable named '" + std::string(childName) + "'");

        int child = int(createChild(currentlyCreating, childName));

        env[childName] = child;

        env["childComponents"].get_or_create<sol::table>()[childName].get_or_create<sol::table>();

        return child;
    };
    env["applyTemplate"] = [&](int extendE, const char *templateName, sol::optional<sol::table> extendArgs) {

        auto entityTemplate = &room->getTemplate(templateName); // could throw error :)

        if (dynamic_cast<LuaEntityTemplate *>(entityTemplate))
        {
            ((LuaEntityTemplate *) entityTemplate)->
                    createComponentsFromScript(entt::entity(extendE), extendArgs);
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
}

void LuaEntityTemplate::createComponents(entt::entity e)
{
    gu::profiler::Zone zone("lua entity script");
    createComponentsFromScript(e, sol::optional<sol::table>());
}

void LuaEntityTemplate::createComponentsFromScript(entt::entity e, sol::optional<sol::table> arguments)
{
    try
    {
        if (currentlyCreating != entt::null)
            throw gu_err("Circular dependency found in EntityTemplate...");
        currentlyCreating = e;

        env["entity"] = e;
        env["components"].get_or_create<sol::table>().clear();
        env["childComponents"].get_or_create<sol::table>().clear();

        if (arguments.has_value())
            env["args"] = arguments;
        else
            env["args"].get_or_create<sol::table>().clear();

        {
            gu::profiler::Zone zone("bytecode");

            // running bytecode instead of text can be 40% faster :O
            getLuaState().script(script->bytecode.as_string_view(), env);
        }

        luaTableToComponents(e, env["components"]);

        auto childComponentsTable = env["childComponents"].get_or_create<sol::table>();

        for (const auto &[childName, compsTable] : childComponentsTable)
        {
            auto childNameStr = childName.as<std::string>();
            auto child = room->getChildByName(e, childNameStr.c_str());
            if (!room->entities.valid(child))
                throw gu_err("Cannot add components to non-existing child named '" + childNameStr + "'. Call createChild(name) first!");
            luaTableToComponents(child, compsTable);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while creating entity using " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    currentlyCreating = entt::null;
}

void LuaEntityTemplate::luaTableToComponents(entt::entity e, const sol::table &table)
{
    gu::profiler::Zone zone("luaTableToComponents");

    for (const auto &[componentName, comp] : table)
    {
        if (!componentName.is<std::string>())
            throw gu_err("All keys in the components table must be a string!");

        auto nameStr = componentName.as<std::string>();

        if (!comp.is<sol::table>())
            throw gu_err("Expected a table for " + nameStr);

        auto utils = ComponentUtils::getFor(nameStr);
        if (!utils)
            throw gu_err("Component named '" + nameStr + "' does not exist!");

        utils->setFromLuaTable(comp.as<sol::table>(), e, room->entities);
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

LuaEntityScript::LuaEntityScript(std::string s) : source(std::move(s))
{
    sol::load_result lr = LuaEntityTemplate::getLuaState().load(source);
    if (!lr.valid())
        throw gu_err("Lua code invalid!");
    bytecode = sol::protected_function(lr).dump();
}
