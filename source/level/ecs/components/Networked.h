
#ifndef GAME_NETWORKED_H
#define GAME_NETWORKED_H


#include "../networking/NetworkedComponent.h"
#include "Physics.h"

struct Networked
{

    std::vector<AbstractNetworkedComponent *>
            toSend,
            toReceive;


};

void poep() {

    Networked p {
            { new NetworkedComponent<Physics>() },
            {}
    };

}

#endif
