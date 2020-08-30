
#include "LuaEntityTemplate.h"
#include "../components/LuaScript.h"
#include "../components/physics/Physics.h"


LuaEntityTemplate::LuaEntityTemplate(const char *assetName, const char *name, Room *r)
    : script(assetName),
      env(luau::getLuaState(), sol::create, luau::getLuaState().globals())
{
    room = r;
    env["TEMPLATE_NAME"] = name;

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
                componentUtils(c);  // will throw error if that type of component does not exist.
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
    env["getTile"] = [&](int x, int y) -> int {
        return int(room->getMap().getTile(x, y));
    };
    // todo: setTile() etc.
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

        auto table = sol::table::create(env.lua_state());
        utils.getToLuaTable(table, entt::entity(entity), room->entities);
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
    env["setUpdateFunction"] = [&](int entity, float updateFrequency, const sol::function &func, sol::optional<bool> randomAcummulationDelay) {

        LuaScripted &scripted = room->entities.get_or_assign<LuaScripted>(entt::entity(entity));
        scripted.updateFrequency = updateFrequency;

        if (randomAcummulationDelay.value_or(true))
            scripted.updateAccumulator = scripted.updateFrequency * mu::random();
        else
            scripted.updateAccumulator = 0;

        scripted.updateFunc = func;
        scripted.updateFuncScript = script;
    };
    env["setOnDestroyCallback"] = [&](int entity, const sol::function &func) {

        LuaScripted &scripted = room->entities.get_or_assign<LuaScripted>(entt::entity(entity));
        scripted.onDestroyFunc = func;
        scripted.onDestroyFuncScript = script;
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
    env["applyTemplate"] = [&](int extendE, const char *templateName, const sol::optional<sol::table> &extendArgs, sol::optional<bool> persistent) {

        auto entityTemplate = &room->getTemplate(templateName); // could throw error :)

        bool makePersistent = persistent.value_or(false);

        if (dynamic_cast<LuaEntityTemplate *>(entityTemplate))
        {
            ((LuaEntityTemplate *) entityTemplate)->
                    createComponentsWithLuaArguments(entt::entity(extendE), extendArgs, makePersistent);
        }
        else
            entityTemplate->createComponents(entt::entity(extendE), makePersistent);
    };
    runScript();
}

void LuaEntityTemplate::runScript()
{
    try
    {
        luau::getLuaState().script(script->getByteCode().as_string_view(), env);
        createFunc = env["create"];
        onDestroyFunc = env["onDestroy"];
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while running template script " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void LuaEntityTemplate::createComponents(entt::entity e, bool persistent)
{
    auto *p = persistent ? room->entities.try_get<Persistent>(e) : NULL;
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

        if (persistent)
        {
            auto &p = room->entities.assign_or_replace<Persistent>(e, persistency);
            if (persistentArgs && arguments.has_value() && arguments.value().valid())
                lua_converter<json>::fromLuaTable(arguments.value(), p.data);
        }

        sol::protected_function_result result = createFunc(e, arguments);
        if (!result.valid())
            throw gu_err(result.get<sol::error>().what());

        if (persistent)
        {
            auto *p = room->entities.try_get<Persistent>(e);
            auto *aabb = p && p->saveSpawnPosition ? room->entities.try_get<AABB>(e) : NULL;
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
