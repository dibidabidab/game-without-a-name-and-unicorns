
#ifndef GAME_NETWORKEDDATA_H
#define GAME_NETWORKEDDATA_H

#include <list>
#include <json.hpp>
#include <gu/game_utils.h>
#include "../../../../entt/src/entt/entity/registry.hpp"

typedef int hash;

struct AbstractNetworkedData
{
    /**
     * Returns a unique ID for each type of data
     */
    virtual hash getDataTypeHash() const = 0;

    /**
     * Returns the name of the data type
     */
    virtual const char *getDataTypeName() const = 0;

    /**
     * Updates entity `e` with the data received in json format.
     */
    virtual void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) = 0;

    virtual void removeData(entt::entity, entt::registry &) = 0;

    /**
     * Converts the data to JSON, only if the data has changed since last time.
     *
     * @param hasChanged        Will be set to true if the data has changed since last time
     * @param componentPresent  Will be set to false if the entity doesn't have the data
     * @param out               Output JSON object
     * @param e                 The entity
     * @param reg               The registry the entity lives in
     */
    virtual void dataToJsonIfChanged(bool &hasChanged, bool &dataPresent, json &out, const entt::entity &e,
                                     entt::registry &reg) = 0;

    virtual void dataToJson(json &out, const entt::entity &e, entt::registry &reg) = 0;

    virtual void update(double deltaTime, const entt::entity &e, entt::registry &reg) {}

    virtual ~AbstractNetworkedData() = default;
};

/**
 * Useful for sending a Component when one of it's values has changed.
 *
 * @tparam Component struct made with the COMPONENT() macro
 */
template<class Component>
struct NetworkedComponent : public AbstractNetworkedData
{
    hash getDataTypeHash() const override
    {
        return Component::COMPONENT_TYPE_HASH;
    }

    const char *getDataTypeName() const override
    {
        return Component::COMPONENT_NAME;
    }

    void removeData(entt::entity entity, entt::registry &registry) override
    {
        registry.remove<Component>(entity);
    }

    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        if (Component *com = reg.try_get<Component>(e))
            com->fromJsonArray(in);
        else
        {
            Component newComponent;
            newComponent.fromJsonArray(in);
            reg.assign<Component>(e, newComponent);
        }
    }

    void dataToJsonIfChanged(bool &hasChanged, bool &componentPresent, json &out, const entt::entity &e,
                             entt::registry &reg) override
    {
        gu::profiler::Zone z("toJson");

        Component *com = reg.try_get<Component>(e);
        componentPresent = com != nullptr;
        hasChanged = false;

        if (!componentPresent) return;

        hash newHash = com->getHash();

        if (newHash != com->prevHash)
        {
            hasChanged = true;
            com->toJsonArray(out);
            com->prevHash = newHash;
        }
    }

    void dataToJson(json &out, const entt::entity &e, entt::registry &reg) override
    {
        Component *com = reg.try_get<Component>(e);
        assert(com != NULL);
        com->toJsonArray(out);
    }

};

template <class Component>
struct InterpolatedNetworkedComponent : NetworkedComponent<Component>
{
    Component goal;
    float progress = 0;

    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        goal.fromJsonArray(in);

        if (!reg.try_get<Component>(e))
        {
            reg.assign<Component>(e, goal);
            return;
        }
        progress = 0;
    }

    void update(double deltaTime, const entt::entity &e, entt::registry &reg) override
    {
        Component *current = reg.try_get<Component>(e);
        if (!current || progress == 1)
            return;

        progress = std::min<float>(1, progress + deltaTime);
        current->interpolate(goal, progress);
    }

};


/**
 * Useful for sending MULTIPLE Components whenever ANY of them has changed.
 *
 * @tparam ComponentTypes structs made with the COMPONENT() macro
 */
//template<typename... ComponentTypes>
//struct NetworkedComponentGroup : public AbstractNetworkedData
//{
//    static constexpr std::size_t nrOfTypes = sizeof...(ComponentTypes);
//
//    std::string combinedName;
//    hash combinedHash = 0;
//
//    NetworkedComponentGroup()
//    {
//        assert(nrOfTypes > 1);
//
//        (interpolateComponent<ComponentTypes>(), ...);
//
//        std::vector<const char *> names = { ComponentTypes::COMPONENT_NAME... };
//        std::vector<hash> hashes = { ComponentTypes::COMPONENT_TYPE_HASH... };
//
//        for (int i = 0; i < nrOfTypes; i++)
//        {
//            if (i != 0) combinedName += '&';
//            combinedName += names[i];
//
//            combinedHash ^= hashes[i] + 0x9e3779b9 + (combinedHash << 6u) + (combinedHash >> 2u);
//        }
//    }
//
//    hash getDataTypeHash() const override
//    {
//        return combinedHash;
//    }
//
//    const char *getDataTypeName() const override
//    {
//        return combinedName.c_str();
//    }
//
//    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
//    {
//        int i = 0;
//        (NetworkedComponent<ComponentTypes>::updateFromNetwork(in.at(i++), e, reg), ...);
//    }
//
//    void removeData(entt::entity entity, entt::registry &registry) override
//    {
//        (registry.remove<ComponentTypes>(entity), ...);
//    }
//
//    template <class Component>
//    void interpolateComponent()
//    {
//        Component a, b, c;
//        interpolate(a, b, c, .4);
//    }
//
//    template <class Component>
//    void check(bool &hasChanged, bool &componentsPresent,
//            const entt::entity &e,
//            entt::registry &reg)
//    {
//        Component *com = reg.try_get<Component>(e);
//        if (com == NULL)
//        {
//            componentsPresent = false;
//            return;
//        }
//
//        hash newHash = com->getHash();
//
//        if (newHash != com->prevHash)
//        {
//            hasChanged = true;
//            com->prevHash = newHash;
//        }
//    }
//
//    void dataToJsonIfChanged(bool &hasChanged, bool &componentsPresent, json &out,
//                            const entt::entity &e,
//                            entt::registry &reg) override
//    {
//        gu::profiler::Zone z("toJson");
//
//        // check if one or more components have changed, AND if components are all present
//        (check<ComponentTypes>(hasChanged, componentsPresent, e, reg), ...);
//
//        if (componentsPresent && hasChanged)
//        {
//            // output to json
//            out = json::array({ reg.get<ComponentTypes>(e)... });
//        }
//    }
//
//    void dataToJson(json &out, const entt::entity &e, entt::registry &reg) override
//    {
//        out = json::array({ reg.get<ComponentTypes>(e)... });
//    }
//};

#endif
