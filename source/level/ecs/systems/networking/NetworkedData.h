
#ifndef GAME_NETWORKEDDATA_H
#define GAME_NETWORKEDDATA_H

#include <json.hpp>

typedef size_t hash;

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

    static void updateFromNetwork(const json &in, const entt::entity &e, entt::registry &reg)
    {
        Component *com = reg.try_get<Component>(e);

        if (!com)
        {
            reg.assign<Component>(e);
            com = reg.try_get<Component>(e);
        }
        com->fromJsonArray(in);
    }

    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        updateFromNetwork(in, e, reg);
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
};


/**
 * Useful for sending MULTIPLE Components whenever ANY of them has changed.
 *
 * @tparam ComponentTypes structs made with the COMPONENT() macro
 */
template<typename... ComponentTypes>
struct NetworkedComponentGroup : public AbstractNetworkedData
{
    static constexpr std::size_t nrOfTypes = sizeof...(ComponentTypes);

    std::string combinedName;
    hash combinedHash = 0;

    NetworkedComponentGroup()
    {
        assert(nrOfTypes > 1);

        std::vector<const char *> names = { ComponentTypes::COMPONENT_NAME... };
        std::vector<hash> hashes = { ComponentTypes::COMPONENT_TYPE_HASH... };

        for (int i = 0; i < nrOfTypes; i++)
        {
            if (i != 0) combinedName += '&';
            combinedName += names[i];

            combinedHash ^= hashes[i] + 0x9e3779b9 + (combinedHash << 6u) + (combinedHash >> 2u);
        }
    }

    hash getDataTypeHash() const override
    {
        return combinedHash;
    }

    const char *getDataTypeName() const override
    {
        return combinedName.c_str();
    }

    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        int i = 0;
        (NetworkedComponent<ComponentTypes>::updateFromNetwork(in.at(i++), e, reg), ...);
    }

    template <class Component>
    void check(bool &hasChanged, bool &componentsPresent,
            const entt::entity &e,
            entt::registry &reg)
    {
        Component *com = reg.try_get<Component>(e);
        if (com == NULL)
        {
            componentsPresent = false;
            return;
        }

        hash newHash = com->getHash();

        if (newHash != com->prevHash)
        {
            hasChanged = true;
            com->prevHash = newHash;
        }
    }

    void dataToJsonIfChanged(bool &hasChanged, bool &componentsPresent, json &out,
                            const entt::entity &e,
                            entt::registry &reg) override
    {
        gu::profiler::Zone z("toJson");

        // check if one or more components have changed, AND if components are all present
        (check<ComponentTypes>(hasChanged, componentsPresent, e, reg), ...);

        if (componentsPresent && hasChanged)
        {
            // output to json
            out = json::array({ reg.get<ComponentTypes>(e)... });
        }
    }
};

#endif
