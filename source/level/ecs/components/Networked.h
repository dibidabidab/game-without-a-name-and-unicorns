
#ifndef GAME_NETWORKED_H
#define GAME_NETWORKED_H


#include <utils/gu_error.h>
#include "../systems/networking/NetworkedData.h"
#include "Physics.h"

class NetworkingSystem;
typedef std::shared_ptr<AbstractNetworkedData> NetworkedData_ptr;

/**
 * List of data types to send/receive
 */
struct NetworkedDataList
{
    bool contains(const char *dataType) const
    {
        for (const NetworkedData_ptr &d : list)
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
        add(new NetworkedComponent<Component>());
    }

    /**
     * Add Components to the list
     * @tparam ComponentTypes stucts made with the COMPONENT() macro
     */
    template <class... ComponentTypes>
    void components()
    {
        (component<ComponentTypes>(), ...);
    }

    template <class Component>
    void interpolatedComponent()
    {
        add(new InterpolatedNetworkedComponent<Component>());
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
    template <class... ComponentTypes>
    void componentGroup()
    {
//        auto group = new NetworkedComponentGroup<ComponentTypes...>();
//        add(group);
    }

  private:
    friend NetworkingSystem;

    std::vector<NetworkedData_ptr> list;
    std::map<int, NetworkedData_ptr> hashToType;

    void add(AbstractNetworkedData *d)
    {
        assert(!contains(d->getDataTypeName()));
        list.push_back(NetworkedData_ptr(d));
        hashToType[d->getDataTypeHash()] = list.back();
    }

};

/**
 * Useful for sending or receiving data associated with an entity over the network
 */
COMPONENT(
    Networked, HASH(networkID),

    FIELD_DEF_VAL(final<int>, networkID, rand()), // used to identify an entity across clients
    FIELD_DEF_VAL(final<int>, templateHash, -1) // used to determine what EntityTemplate to use to construct the entity at the client-side
)

    NetworkedDataList toSend, toReceive;

    std::map<size_t, bool> dataPresence;

END_COMPONENT(Networked)

#endif
