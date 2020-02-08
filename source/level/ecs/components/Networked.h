
#ifndef GAME_NETWORKED_H
#define GAME_NETWORKED_H


#include <utils/gu_error.h>
#include "../networking/NetworkedComponent.h"
#include "Physics.h"

struct NetworkedComponents_
{
    std::vector<AbstractNetworkedComponent *> list;

    ~NetworkedComponents_()
    {
//        for (auto *c : list) delete c;
    }
};

typedef std::shared_ptr<NetworkedComponents_> NetworkedComponents;

struct Networked
{

    NetworkedComponents toSend, toReceive;

    int networkID = rand();

    std::map<size_t, bool> componentPresence;

    template <typename... ComponentTypes>
    static NetworkedComponents components()
    {
        return std::make_shared<NetworkedComponents_>(NetworkedComponents_{{ new NetworkedComponent<ComponentTypes>()... }});
    }

};

#endif
