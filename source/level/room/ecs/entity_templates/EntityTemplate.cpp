#include "EntityTemplate.h"
#include "../../Room.h"
#include "../components/Networked.h"
#include "../components/Children.h"

entt::entity EntityTemplate::create(bool persistent)
{
    entt::entity e = room->entities.create();
    createComponents(e, persistent);
    return e;
}

entt::entity EntityTemplate::createNetworked(int networkID, bool serverSide, bool persistent)
{
    auto e = create(persistent);
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
