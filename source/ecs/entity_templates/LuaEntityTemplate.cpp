
#include "LuaEntityTemplate.h"
#include "../components/LuaScripted.h"
#include "../components/physics/Physics.h"
#include "../../game/Game.h"


LuaEntityTemplate::LuaEntityTemplate(const char *assetName, const char *name, EntityEngine *engine_)
    : script(assetName), name(name),
      env(engine_->luaEnvironment.lua_state(), sol::create, engine_->luaEnvironment)
{
    this->engine = engine_; // DONT RENAME engine_ to engine!!!, lambdas should use this->engine.
    env["TEMPLATE_NAME"] = name;

    defaultArgs = sol::table::create(env.lua_state());

    int
        TEMPLATE = env["TEMPLATE"] = 1 << 0,
        ARGS = env["ARGS"] = 1 << 1,
        FINAL_POS = env["FINAL_POS"] = 1 << 2,
        SPAWN_POS = env["SPAWN_POS"] = 1 << 3,
        ALL_COMPONENTS = env["ALL_COMPONENTS"] = 1 << 4,
        REVIVE = env["REVIVE"] = 1 << 5;

    auto setPersistentMode = [&, name](int mode, sol::optional<std::vector<std::string>> componentsToSave) {

        persistency = Persistent();
        if (mode & TEMPLATE)
            persistency.applyTemplateOnLoad = name;

        persistentArgs = mode & ARGS;
        persistency.saveFinalPosition = mode & FINAL_POS;
        persistency.saveSpawnPosition = mode & SPAWN_POS;
        persistency.saveAllComponents = mode & ALL_COMPONENTS;
        persistency.revive = mode & REVIVE;
        if (componentsToSave.has_value())
        {
            persistency.saveComponents = componentsToSave.value();
            for (auto &c : persistency.saveComponents)
                engine->componentUtils(c);  // will throw error if that type of component does not exist.
        }
    };
    env["persistenceMode"] = setPersistentMode;
    setPersistentMode(TEMPLATE | ARGS | FINAL_POS, sol::optional<std::vector<std::string>>());

    env["defaultArgs"] = [&](const sol::table &table) {
        defaultArgs = table;
    };
    env["description"] = [&](const char *d) {
        description = d;
    };
    env["setUpdateFunction"] = [&](int entity, float updateFrequency, const sol::function &func, sol::optional<bool> randomAcummulationDelay) {

        LuaScripted &scripted = engine->entities.get_or_assign<LuaScripted>(entt::entity(entity));
        scripted.updateFrequency = updateFrequency;

        if (randomAcummulationDelay.value_or(true))
            scripted.updateAccumulator = scripted.updateFrequency * mu::random();
        else
            scripted.updateAccumulator = 0;

        scripted.updateFunc = func;
        scripted.updateFuncScript = script;
    };
    env["setOnDestroyCallback"] = [&](int entity, const sol::function &func) {

        LuaScripted &scripted = engine->entities.get_or_assign<LuaScripted>(entt::entity(entity));
        scripted.onDestroyFunc = func;
        scripted.onDestroyFuncScript = script;
    };

    runScript();
}

void LuaEntityTemplate::runScript()
{
    try
    {
        // todo: use same lua_state as 'env' is in
        luau::getLuaState().unsafe_script(script->getByteCode().as_string_view(), env);
        createFunc = env["create"];
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while running template script " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void LuaEntityTemplate::createComponents(entt::entity e, bool persistent)
{
    auto *p = persistent ? engine->entities.try_get<Persistent>(e) : NULL;
    if (p)
        createComponentsWithJsonArguments(e, p->data, true);
    else
        createComponentsWithLuaArguments(e, sol::optional<sol::table>(), persistent);
}

void LuaEntityTemplate::createComponentsWithLuaArguments(entt::entity e, sol::optional<sol::table> arguments, bool persistent)
{
    if (script.hasReloaded())
        runScript();

    try
    {
        if (arguments.has_value() && defaultArgs.valid())
        {
            for (auto &[key, defaultVal] : defaultArgs)
            {
                if (!arguments.value()[key].valid())
                    arguments.value()[key] = defaultVal;
            }
        } else arguments = defaultArgs;

        LuaScripted& luaScripted = engine->entities.get_or_assign<LuaScripted>(e);

        std::string id = arguments.value()["saveGameEntityID"].get_or<std::string, std::string>(getUniqueID());
        luaScripted.saveData = Game::getCurrentSession().saveGame.getSaveDataForEntity(id, !persistent);

        if (persistent)
        {
            auto &p = engine->entities.assign_or_replace<Persistent>(e, persistency);
            if (persistentArgs && arguments.has_value() && arguments.value().valid())
                lua_converter<json>::fromLuaTable(arguments.value(), p.data);

            assert(p.data.is_object());
            p.data["saveGameEntityID"] = id;
        }

        sol::protected_function_result result = createFunc(e, arguments, luaScripted.saveData);
        if (!result.valid())
            throw gu_err(result.get<sol::error>().what());

        if (persistent)
        {
            auto *p = engine->entities.try_get<Persistent>(e);
            auto *aabb = p && p->saveSpawnPosition ? engine->entities.try_get<AABB>(e) : NULL;
            if (aabb)
                p->spawnPosition = aabb->center;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while creating entity using " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
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
    lua_converter<json>::fromLua(defaultArgs, j);
    return j;
}

void LuaEntityTemplate::createComponentsWithJsonArguments(entt::entity e, const json &arguments, bool persistent)
{
    auto table = sol::table::create(env.lua_state());
    if (arguments.is_structured())
        lua_converter<json>::toLuaTable(table, arguments);
    createComponentsWithLuaArguments(e, table, persistent);
}

std::string LuaEntityTemplate::getUniqueID()
{
    return name + "_" + randomString(24);
}
