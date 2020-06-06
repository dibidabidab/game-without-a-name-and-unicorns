
#include "LuaEntityTemplate.h"
#include "../../../macro_magic/component.h"

LuaEntityTemplate::LuaEntityTemplate(const char *assetName) : script(assetName)
{

}

entt::entity LuaEntityTemplate::create()
{
    sol::state lua;

    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);

    return create(lua, sol::optional<sol::table>());
}

entt::entity LuaEntityTemplate::create(sol::state &lua, sol::optional<sol::table> arguments)
{
    sol::environment env(lua, sol::create, lua.globals());

    entt::entity e = entt::null;
    try
    {
        env["args"] = arguments;
        env["create"] = [&]() -> int { return int(e = room->entities.create()); };
        env["extend"] = [&](const char *templateName, sol::optional<sol::table> extendArgs) -> int {

            auto entityTemplate = room->getTemplate(templateName);

            if (dynamic_cast<LuaEntityTemplate *>(entityTemplate))
            {
                return int(((LuaEntityTemplate *) entityTemplate)->create(lua, extendArgs));
            }
            return int(entityTemplate->create());
        };

        lua.script(script->source, env);

        if (e == entt::null) // create() was not called
            e = room->entities.create();

        sol::table componentsTable = env["components"];

        for (const auto &[componentName, val] : componentsTable)
        {
            sol::table comp = val;

            std::cout << componentName.as<std::string>() << '\n';

            for (const auto &[key, val] : comp)
                std::cout << key.as<std::string>() << " = " << val.as<std::string>() << "\n";

        }

    }
    catch (std::exception &e)
    {
        std::cerr << "Error while creating entity using " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return e;
}
