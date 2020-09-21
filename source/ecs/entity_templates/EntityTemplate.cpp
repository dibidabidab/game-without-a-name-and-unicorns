#include "EntityTemplate.h"
#include "../EntityEngine.h"
#include "../../generated/Networked.hpp"

entt::entity EntityTemplate::create(bool persistent)
{
    entt::entity e = engine->entities.create();
    createComponents(e, persistent);
    return e;
}

entt::entity EntityTemplate::createNetworked(int networkID, bool serverSide, bool persistent)
{
    auto e = create(persistent);
    engine->entities.assign<Networked>(e, networkID, templateHash);

    Networked &n = engine->entities.get<Networked>(e);

    if (serverSide)
        makeNetworkedServerSide(n);
    else
        makeNetworkedClientSide(n);

    return e;
}
