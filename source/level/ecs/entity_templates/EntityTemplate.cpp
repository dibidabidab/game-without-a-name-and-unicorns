#include "EntityTemplate.h"
#include "../../room/Room.h"
#include "../components/Networked.h"
#include "../components/Children.h"

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

void EntityTemplate::setParent(entt::entity child, entt::entity parent, const char *childName)
{
    room->entities.assign<Child>(child, parent, childName);
}

entt::entity EntityTemplate::createChild(entt::entity parent, const char *childName)
{
    entt::entity e = room->entities.create();
    setParent(e, parent, childName);
    return e;
}
