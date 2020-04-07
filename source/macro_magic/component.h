
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "json_reflectable.h"
#include <utils/hashing.h>

struct ComponentUtils
{
  public:

    std::function<bool(entt::entity, const entt::registry *)> entityHasComponent;
    std::function<void(json &, entt::entity, const entt::registry *)> getJsonComponent;
    std::function<void(const json &, const entt::entity &, entt::registry *)> setJsonComponent;
    std::function<void(entt::entity, entt::registry *)> addComponent;
    std::function<void(entt::entity, entt::registry *)> removeComponent;

    template <class Component>
    static ComponentUtils *create()
    {
        auto u = new ComponentUtils();
        utils[Component::COMPONENT_NAME] = u;
        names.push_back(Component::COMPONENT_NAME);

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
        u->addComponent = [] (entt::entity e, entt::registry *reg)
        {
            reg->assign<Component>(e);
        };
        u->removeComponent = [] (entt::entity e, entt::registry *reg)
        {
            reg->remove<Component>(e);
        };
        return u;
    }

    static const ComponentUtils *getFor(const char *componentName)
    {
        return utils[componentName];
    }

    static const std::vector<const char *> getAllComponentTypeNames()
    {
        return names;
    }

  private:
    static inline std::map<const char *, ComponentUtils *> utils;
    static inline std::vector<const char *> names;

};

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
    REFLECTABLE_STRUCT(component_name, __VA_ARGS__)\
        \
        constexpr static const char *COMPONENT_NAME = #component_name;\
        \
        inline static const size_t COMPONENT_TYPE_HASH = hashStringCrossPlatform(#component_name);\
        \
        inline static const ComponentUtils *UTILS = ComponentUtils::create<component_name>();\
        \
        hash_func\

#define END_COMPONENT(component_name)\
        size_t prevHash = 0;\
        END_REFLECTABLE_STRUCT(component_name)

COMPONENT(
    // component name:
    Test,

    // values to calculate hash:
    HASH(ham, kaas),

    // fields:
    FIELD_DEF_VAL (int, ham, 2),
    FIELD         (float, kaas)
)
END_COMPONENT(Test)

#endif //GAME_COMPONENT_H
