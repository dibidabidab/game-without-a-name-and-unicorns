
#ifndef GAME_NETWORKEDDATALIST_H
#define GAME_NETWORKEDDATALIST_H

#include <utils/gu_error.h>
#include "NetworkedData.h"

class NetworkedDataGroup;
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
    NetworkedDataList &component()
    {
        add(new NetworkedComponent<Component>());
        return *this;
    }

    /**
     * Add Components to the list
     * @tparam ComponentTypes stucts made with the COMPONENT() macro
     */
    template <class... ComponentTypes>
    NetworkedDataList &components()
    {
        (component<ComponentTypes>(), ...);
        return *this;
    }

    template <class Component>
    NetworkedDataList &interpolatedComponent()
    {
        add(new InterpolatedNetworkedComponent<Component>());
        return *this;
    }

    /**
     * Add a Component-group to the list
     *
     * A Component-group
     *      - sends ALL components when one or more have changed
     *      - receives ALL components in 1 packet
     *
     */
    NetworkedDataGroup &group();

    virtual void endGroup()
    {
        throw gu_err("this is not a group");
    }

  protected:
    friend NetworkingSystem;
    friend NetworkedDataGroup;

    std::vector<NetworkedData_ptr> list;
    std::map<int, NetworkedData_ptr> hashToType;

    virtual void add(AbstractNetworkedData *d)
    {
        assert(!contains(d->getDataTypeName()));
        list.push_back(NetworkedData_ptr(d));
        hashToType[d->getDataTypeHash()] = list.back();
    }

};

#endif
