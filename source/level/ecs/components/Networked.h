
#ifndef GAME_NETWORKED_H
#define GAME_NETWORKED_H


#include <utils/gu_error.h>
#include "../systems/networking/NetworkedData.h"
#include "Physics.h"

class NetworkingSystem;

/**
 * List of data types to send/receive
 */
struct NetworkedDataList
{
    bool contains(const char *dataType) const
    {
        for (const AbstractNetworkedData *d : list)
            if (strcmp(d->getDataTypeName(), dataType) == 0) return true;
        return false;
    }

    /**
     * Add a Component to the list
     * @tparam Component stuct made with the COMPONENT() macro
     */
    template <class Component>
    void component()
    {
        auto c = new NetworkedComponent<Component>();
        add(c);
        destructors.push_back([=] {
            delete c;
        });
    }

    /**
     * Add Components to the list
     * @tparam ComponentTypes stucts made with the COMPONENT() macro
     */
    template <typename... ComponentTypes>
    void components()
    {
        (component<ComponentTypes>(), ...);
    }

    /**
     * Add a Component-group to the list
     *
     * A Component-group
     *      - sends ALL components when one or more have changed
     *      - receives ALL components in 1 packet
     *
     * @tparam ComponentTypes stucts made with the COMPONENT() macro
     */
    template <typename... ComponentTypes>
    void componentGroup()
    {
        auto group = new NetworkedComponentGroup<ComponentTypes...>();
        add(group);
        destructors.push_back([=] {
            delete group;
        });
    }

    ~NetworkedDataList()
    {
        for (auto &d : destructors) d();
    }

  private:
    friend NetworkingSystem;

    std::vector<AbstractNetworkedData *> list;
    std::map<int, AbstractNetworkedData *> hashToType;

    void add(AbstractNetworkedData *d)
    {
        assert(!contains(d->getDataTypeName()));
        list.push_back(d);
        hashToType[d->getDataTypeHash()] = d;
    }

    std::vector<std::function<void()>> destructors;
};

/**
 * Useful for sending or receiving data associated with an entity over the network
 */
COMPONENT(
    Networked, HASH(networkID),

    FIELD_DEF_VAL(final<int>, networkID, rand()), // used to identify an entity across clients
    FIELD_DEF_VAL(final<int>, templateHash, -1) // used to determine what EntityTemplate to use to construct the entity at the client-side
)

    std::shared_ptr<NetworkedDataList> toSend, toReceive;

    std::map<size_t, bool> dataPresence;

END_COMPONENT(Networked)

#endif
