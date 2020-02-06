
#ifndef GAME_NETWORKEDCOMPONENT_H
#define GAME_NETWORKEDCOMPONENT_H

#include <json.hpp>

typedef size_t hash;

struct AbstractNetworkedComponent
{
    /**
     * Returns a unique ID for each type of component
     */
    virtual hash getComponentTypeHash() const = 0;

    /**
     * Returns the name of the component type
     */
    virtual const char *getComponentTypeName() const = 0;

    /**
     * Updates (or creates) the component (which belongs to entity `e`) with the data received in json format.
     */
    virtual void updateComponentFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) = 0;

    /**
     * Converts the component to JSON, only if the component has changed since last time.
     *
     * @param hasChanged        Will be set to true if the component has changed since last time
     * @param componentPresent  Will be set to false if the entity doesn't have the component
     * @param out               Output JSON object
     * @param e                 The entity
     * @param reg               The registry the entity lives in
     */
    virtual void componentToJsonIfChanged(bool &hasChanged, bool &componentPresent, json &out, const entt::entity &e,
                                          entt::registry &reg) = 0;

    virtual ~AbstractNetworkedComponent() = default;
};


template<class Component>
struct NetworkedComponent : public AbstractNetworkedComponent
{
    hash getComponentTypeHash() const override
    {
        return Component::COMPONENT_TYPE_HASH;
    }

    const char *getComponentTypeName() const override
    {
        return Component::COMPONENT_NAME;
    }

    void updateComponentFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        Component *com = reg.try_get<Component>(e);

        if (!com)
        {
            reg.assign<Component>(e);
            com = reg.try_get<Component>(e);
        }
        com->updateFromJson(in);
    }

    void componentToJsonIfChanged(bool &hasChanged, bool &componentPresent, json &out, const entt::entity &e,
                                  entt::registry &reg) override
    {
        Component *com = reg.try_get<Component>(e);
        componentPresent = com != nullptr;
        hasChanged = false;

        if (!componentPresent) return;

        hash newHash = com->getHash();

        if (newHash != com->prevHash)
        {
            hasChanged = true;
            com->toJson(out);
            com->prevHash = newHash;
        }
    }
};

#endif
