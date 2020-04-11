#include "EntityTemplate.h"
#include "../../room/Room.h"
#include "../components/Networked.h"

entt::entity EntityTemplate::createNetworked(int networkID, bool serverSide)
{
    auto e = create();
    room->entities.assign<Networked>(e, networkID, templateHash);

    Networked &n = room->entities.get<Networked>(e);

    if (serverSide)
        makeNetworkedServerSide(n);
    else
        makeNetworkedClientSide(n);

    return e;
}
