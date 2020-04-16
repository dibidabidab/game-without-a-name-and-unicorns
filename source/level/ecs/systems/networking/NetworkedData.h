
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
     * @param dataPresent       Will be set to false if the entity doesn't have the data
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
        Component *com = reg.try_get<Component>(e);
        if (com == NULL)
        {
            componentPresent = false;
            return;
        }

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
struct ComponentInterpolator
{
    float speed = 1.;

    Component diff(Component &c, const Component &other);

    void add(Component &c, const Component &diff, float multiplier);
};

template <class Component>
struct InterpolatedNetworkedComponent : NetworkedComponent<Component>
{
    bool firstUpdateReceived = false;
    Component diff;
    ComponentInterpolator<Component> interpolator;
    float progress = 0;

    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        Component newC;
        newC.fromJsonArray(in);
        Component *current = reg.try_get<Component>(e);

        if (!current)
        {
            reg.assign<Component>(e, newC);
            return;
        }
        interpolator = ComponentInterpolator<Component>();
        diff = interpolator.diff(*current, newC);
        firstUpdateReceived = true;
        progress = 0;
    }

    void update(double deltaTime, const entt::entity &e, entt::registry &reg) override
    {
        if (!firstUpdateReceived)
            return;
        Component *current = reg.try_get<Component>(e);
        if (!current || progress == 1)
            return;

        float newProgress = deltaTime * interpolator.speed;
        if (progress + newProgress > 1)
            newProgress = 1 - progress;
        interpolator.add(*current, diff, newProgress);
        progress += newProgress;
    }

};

#endif
