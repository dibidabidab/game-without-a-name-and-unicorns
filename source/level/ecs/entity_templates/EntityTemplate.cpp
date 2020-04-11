#include "EntityTemplate.h"
#include "../../room/Room.h"
#include "../components/Networked.h"

entt::entity EntityTemplate::createNetworked(int networkID, bool serverSide)
{
    auto e = create();
    room->entities.assign<Networked>(e, networkID);

    Networked &n = room->entities.get<Networked>(e);
    n.templateHash = templateHash;

    if (serverSide)
        makeNetworkedServerSide(n);
    else
        makeNetworkedClientSide(n);

    return e;
}
