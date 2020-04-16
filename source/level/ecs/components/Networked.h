
#ifndef GAME_NETWORKED_H
#define GAME_NETWORKED_H

#include "../../../macro_magic/component.h"
#include "../systems/networking/NetworkedDataList.h"
#include "../systems/networking/NetworkedDataGroup.h"

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
