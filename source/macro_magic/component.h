
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "../../entt/src/entt/entity/registry.hpp"
#include "serializable.h"
#include <utils/hashing.h>

struct ComponentUtils
{
  public:

    std::function<bool(entt::entity, const entt::registry *)> entityHasComponent;
    std::function<void(json &, entt::entity, const entt::registry *)> getJsonComponent;
    std::function<void(const json &, entt::entity, entt::registry *)> setJsonComponent;
    std::function<void(const json &, entt::entity, entt::registry *)> addComponent;
    std::function<void(entt::entity, entt::registry *)> removeComponent;
    std::function<json()> construct;

    template <class Component>
    const static ComponentUtils *create()
    {
        if (!utils) utils = new std::map<const char *, ComponentUtils *>();
        if (!names) names = new std::vector<const char *>();

        // for some reason ComponentUtils::create<Component>() is called multiple times for the same Component on Windows.... wtf...
        if (getFor(Component::COMPONENT_NAME))
            return getFor(Component::COMPONENT_NAME);

        auto u = new ComponentUtils();

        utils->operator[](Component::COMPONENT_NAME) = u;
        names->push_back(Component::COMPONENT_NAME);

        u->entityHasComponent = [] (entt::entity e, const entt::registry *reg)
        {
            return reg->try_get<Component>(e) != NULL;
        };
        u->getJsonComponent = [] (json &j, entt::entity e, const entt::registry *reg)
        {
            reg->get<Component>(e).toJsonArray(j);
        };
        u->setJsonComponent = [] (const json &j, entt::entity e, entt::registry *reg)
        {
            reg->get<Component>(e).fromJsonArray(j);
        };
        u->addComponent = [] (const json &json, entt::entity e, entt::registry *reg)
        {
            Component c = json;
            reg->assign<Component>(e, c);
        };
        u->removeComponent = [] (entt::entity e, entt::registry *reg)
        {
            reg->remove<Component>(e);
        };
        u->construct = [] { return Component(); };
        return u;
    }

    static const ComponentUtils *getFor(const char *componentName)
    {
        return utils->operator[](componentName);
    }

    static const std::vector<const char *> &getAllComponentTypeNames()
    {
        return *names;
    }

  private:
    static std::map<const char *, ComponentUtils *> *utils;
    static std::vector<const char *> *names;

};

template <typename Type>
using final = Type;

#define HASH_FIELD(X) \
    hash ^= hashValue(X) + 0x9e3779b9 + (hash << 6u) + (hash >> 2u)

#define HASH(...)\
    size_t getHash()\
    {\
        size_t hash = 0;\
        DOFOREACH_SEMICOLON(HASH_FIELD, __VA_ARGS__)\
        return hash;\
    }\


#define COMPONENT(component_name, hash_func, ...)\
    SERIALIZABLE(component_name, __VA_ARGS__)\
        \
        constexpr static const char *COMPONENT_NAME = #component_name;\
        \
        inline static const int COMPONENT_TYPE_HASH = hashStringCrossPlatform(#component_name);\
        \
        inline static const ComponentUtils *UTILS = ComponentUtils::create<component_name>();\
        \
        hash_func\

#define END_COMPONENT(component_name)\
        size_t prevNetworkedHash = -123456;\
        END_SERIALIZABLE(component_name)

#endif //GAME_COMPONENT_H
