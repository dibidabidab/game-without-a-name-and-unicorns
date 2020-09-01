
#include "EntityEngine.h"
#include "components/Children.h"
#include <utils/hashing.h>
#include <utils/gu_error.h>
#include "entity_templates/LuaEntityTemplate.h"

void EntityEngine::addSystem(EntitySystem *sys)
{
    assert(!initialized);
    systems.push_back(sys);
}

EntityTemplate &EntityEngine::getTemplate(std::string name)
{
    try {
        return getTemplate(hashStringCrossPlatform(name));
    }
    catch (_gu_err &)
    {
        throw gu_err("No EntityTemplate named " + name + " found");
    }
}

EntityTemplate &EntityEngine::getTemplate(int templateHash)
{
    auto t = entityTemplates[templateHash];
    if (!t)
        throw gu_err("No EntityTemplate found for hash " + std::to_string(templateHash));
    return *t;
}

const std::vector<std::string> &EntityEngine::getTemplateNames() const
{
    return entityTemplateNames;
}

entt::entity EntityEngine::getChildByName(entt::entity parent, const char *childName)
{
    const Parent *p = entities.try_get<Parent>(parent);
    if (!p)
        return entt::null;
    auto it = p->childrenByName.find(childName);
    if (it == p->childrenByName.end())
        return entt::null;
    return it->second;
}

void EntityEngine::registerLuaEntityTemplate(const char *assetPath)
{
    auto name = splitString(assetPath, "/").back();

    addEntityTemplate(name, new LuaEntityTemplate(assetPath, name.c_str(), this));
}

void EntityEngine::addEntityTemplate(const std::string &name, EntityTemplate *t)
{
    int hash = hashStringCrossPlatform(name);

    bool replace = entityTemplates[hash] != NULL;

    delete entityTemplates[hash];
    auto et = entityTemplates[hash] = t;
    et->engine = this;
    et->templateHash = hash;

    if (!replace)
        entityTemplateNames.push_back(name);
}

EntityEngine::~EntityEngine()
{
    for (auto sys : systems)
        delete sys;
    for (auto &entry : entityTemplates)
        delete entry.second;
}

void EntityEngine::initialize()
{
    assert(!initialized);

    initializeLuaEnvironment();

    for (auto &el : AssetManager::getLoadedAssetsForType<luau::Script>())
        if (stringStartsWith(el.first, "scripts/entities/"))
            registerLuaEntityTemplate(el.second->shortPath.c_str());

    for (auto sys : systems)
        sys->init(this);

    initialized = true;
}

void EntityEngine::initializeLuaEnvironment()
{
    // todo: functions might be called after EntityEngine is destructed

    luaEnvironment = sol::environment(luau::getLuaState(), sol::create, luau::getLuaState().globals());
    auto &env = luaEnvironment;

    env["getComponent"] = [&](int entity, const std::string &componentName) -> sol::optional<sol::table> {

        auto &utils = componentUtils(componentName);

        if (!utils.entityHasComponent(entt::entity(entity), entities))
            return sol::optional<sol::table>();

        auto table = sol::table::create(env.lua_state());
        utils.getToLuaTable(table, entt::entity(entity), entities);
        return table;
    };
    env["removeComponent"] = [&](int entity, const std::string &componentName) {
        componentUtils(componentName).removeComponent(entt::entity(entity), entities);
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

    env["createEntity"] = [&]() -> int {

        return int(entities.create());
    };
    env["destroyEntity"] = [&](int e) {

        entities.destroy(entt::entity(e));
    };
    env["createChild"] = [&](int parentEntity, sol::optional<std::string> childName) -> int {

        int child = int(createChild(entt::entity(parentEntity), childName.has_value() ? childName.value().c_str() : ""));
        return child;
    };
    env["getChild"] = [&](int parentEntity, const char *childName) -> sol::optional<int> {

        entt::entity childEntity = getChildByName(entt::entity(parentEntity), childName);
        if (childEntity == entt::null)
            return sol::optional<int>(); // nil
        return int(childEntity);
    };
    env["applyTemplate"] = [&](int extendE, const char *templateName, const sol::optional<sol::table> &extendArgs, sol::optional<bool> persistent) {

        auto entityTemplate = &getTemplate(templateName); // could throw error :)

        bool makePersistent = persistent.value_or(false);

        if (dynamic_cast<LuaEntityTemplate *>(entityTemplate))
        {
            ((LuaEntityTemplate *) entityTemplate)->
                    createComponentsWithLuaArguments(entt::entity(extendE), extendArgs, makePersistent);
        }
        else
            entityTemplate->createComponents(entt::entity(extendE), makePersistent);
    };
}

void EntityEngine::luaTableToComponent(entt::entity e, const std::string &componentName, const sol::table &component)
{
    componentUtils(componentName).setFromLuaTable(component, e, entities);
}

// todo: ComponentUtils::getFor() and ComponentUtils::tryGetFor()
const ComponentUtils &EntityEngine::componentUtils(const std::string &componentName)
{
    auto utils = ComponentUtils::getFor(componentName);
    if (!utils)
        throw gu_err("Component-type named '" + componentName + "' does not exist!");
    return *utils;
}

void EntityEngine::setParent(entt::entity child, entt::entity parent, const char *childName)
{
    entities.assign<Child>(child, parent, childName);
}

entt::entity EntityEngine::createChild(entt::entity parent, const char *childName)
{
    entt::entity e = entities.create();
    setParent(e, parent, childName);
    return e;
}

