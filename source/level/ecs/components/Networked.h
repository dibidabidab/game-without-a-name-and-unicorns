
#ifndef GAME_NETWORKED_H
#define GAME_NETWORKED_H


#include <utils/gu_error.h>
#include "../networking/NetworkedComponent.h"
#include "Physics.h"

typedef std::vector<AbstractNetworkedComponent *> NetworkedComponents;

struct Networked
{

    NetworkedComponents toSend, toReceive;

    std::map<size_t, bool> componentPresence;

    Networked(NetworkedComponents toSend, NetworkedComponents toReceive)
        : toSend(toSend), toReceive(toReceive) {}

    Networked(const Networked &n)
    {
        throw gu_err("yeah, this is gonna break");
    }

    ~Networked()
    {
        for (auto *c : toSend)    delete c;
        for (auto *c : toReceive) delete c;
    }

    template <typename... ComponentTypes>
    static NetworkedComponents components()
    {
        return { new NetworkedComponent<ComponentTypes>()... };
    }

};

#endif
