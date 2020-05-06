
#ifndef GAME_PLAYERENTITY_H
#define GAME_PLAYERENTITY_H


#include "EntityTemplate.h"
#include "../components/Physics.h"
#include "../components/PlatformerMovement.h"
#include "../components/Networked.h"
#include "../components/PlayerControlled.h"
#include "../components/Light.h"

class PlayerEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<Physics>(e);
        room->entities.assign<AABB>(e, ivec2(5, 13), ivec2(32, 52));
        room->entities.assign<StaticCollider>(e);
        room->entities.assign<PlatformerMovement>(e);
        room->entities.assign<LightPoint>(e);
        return e;
    }

    void makeNetworkedServerSide(Networked &networked) override
    {
        networked.toSend.components<PlayerControlled, PlatformerMovement>();

        networked.toSend.group()
                .component<Physics>()
                .interpolatedComponent<AABB>()
                .endGroup();

        networked.toReceive.group()
                .component<Physics>()
                .interpolatedComponent<AABB>()
                .endGroup();
    }

    void makeNetworkedClientSide(Networked &networked) override
    {
        networked.toReceive.components<PlayerControlled, PlatformerMovement>();

        networked.sendIfLocalPlayerReceiveOtherwise.group()
                .component<Physics>()
                .interpolatedComponent<AABB>()
                .endGroup();
    }
};


#endif
