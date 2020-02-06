
#ifndef GAME_NETWORKEDCOMPONENT_H
#define GAME_NETWORKEDCOMPONENT_H

#include <json.hpp>

typedef size_t hash;

struct AbstractNetworkedComponent
{
    virtual hash getComponentTypeHash() const = 0;

    virtual void updateComponentFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) = 0;

    virtual bool componentToJsonIfHashChanged(json &out, const entt::entity &e, entt::registry &reg) = 0;

    virtual void componentToJson(json &out, const entt::entity &e, entt::registry &reg) = 0;
};

template <class Component>
struct NetworkedComponent : public AbstractNetworkedComponent
{
    hash getComponentTypeHash() const override
    {
        return Component::COMPONENT_TYPE_HASH;
    }

    void updateComponentFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        reg.get<Component>(e).updateFromJson(in);
    }

    bool componentToJsonIfHashChanged(json &out, const entt::entity &e, entt::registry &reg) override
    {
        Component &com = reg.get<Component>(e);
        hash newHash = com.getHash();

        if (newHash != com.prevHash)
        {
            com.toJson(out);
            com.prevHash = newHash;
            return true;
        }
        return false;
    }

    void componentToJson(json &out, const entt::entity &e, entt::registry &reg) override
    {
        reg.get<Component>(e).toJson(out);
    }
};

#endif
