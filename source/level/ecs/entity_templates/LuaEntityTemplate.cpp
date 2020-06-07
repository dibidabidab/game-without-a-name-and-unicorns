
#include "LuaEntityTemplate.h"
#include "../../../macro_magic/component.h"

LuaEntityTemplate::LuaEntityTemplate(const char *assetName) : script(assetName)
{

}

void LuaEntityTemplate::createComponents(entt::entity e)
{
    sol::state lua;

    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);

    createComponentsFromScript(e, lua, sol::optional<sol::table>());
}

void LuaEntityTemplate::createComponentsFromScript(entt::entity e, sol::state &lua, sol::optional<sol::table> arguments)
{
    sol::environment env(lua, sol::create, lua.globals());

    try
    {
        env["entity"] = e;
        env["components"].get_or_create<sol::table>();

        if (arguments.has_value())
            env["args"] = arguments;
        else
            env["args"].get_or_create<sol::table>();

        env["arg"] = [&](const char *argName, sol::optional<sol::reference> defaultVal) {
            if (!env["args"][argName].valid())
                env["args"][argName] = defaultVal;
        };
        env["createChild"] = [&](const char *childName) -> int {

            if (env[childName].valid())
                throw gu_err("Could not create child.\nIt seems that the lua script already has a variable named '" + std::string(childName) + "'");

            int child = int(createChild(e, childName));

            env[childName] = child;

            env["childComponents"].get_or_create<sol::table>()[childName].get_or_create<sol::table>();

            return child;
        };
        env["applyTemplate"] = [&](int extendE, const char *templateName, sol::optional<sol::table> extendArgs) {

            auto entityTemplate = &room->getTemplate(templateName); // could throw error :)

            if (dynamic_cast<LuaEntityTemplate *>(entityTemplate))
            {
                ((LuaEntityTemplate *) entityTemplate)->
                    createComponentsFromScript(entt::entity(extendE), lua, extendArgs);
            }
            else
                entityTemplate->createComponents(entt::entity(extendE));
        };

        lua.script(script->source, env);

        luaTableToComponents(e, env["components"]);

        auto childComponentsTable = env["childComponents"].get_or_create<sol::table>();

        for (const auto &[childName, compsTable] : childComponentsTable)
        {
            auto child = room->getChildByName(e, childName.as<std::string>().c_str());
            luaTableToComponents(child, compsTable);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while creating entity using " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void LuaEntityTemplate::luaTableToComponents(entt::entity e, const sol::table &table)
{
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
