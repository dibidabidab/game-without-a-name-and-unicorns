
#ifndef GAME_PLAYERENTITY_H
#define GAME_PLAYERENTITY_H


#include "EntityTemplate.h"
#include "../components/Physics.h"
#include "../components/PlatformerMovement.h"
#include "../components/Networked.h"
#include "../components/PlayerControlled.h"

class PlayerEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<Physics>(e);
        room->entities.assign<AABB>(e, ivec2(5, 13), ivec2(32, 52));
        room->entities.assign<PlatformerMovement>(e);
        return e;
    }

    void makeNetworkedServerSide(Networked &networked) override
    {
        networked.toSend.component<PlayerControlled>();
        networked.toSend.component<PlatformerMovement>();
        networked.toSend.component<AABB>();
        networked.toSend.component<Physics>();

//        toSend.componentGroup<Physics, AABB>(); // if any changes -> send all
    }

    void makeNetworkedClientSide(Networked &networked) override
    {
        networked.toReceive.component<PlayerControlled>();
        networked.toReceive.component<PlatformerMovement>();
        networked.toReceive.interpolatedComponent<AABB>();
        networked.toReceive.component<Physics>();
    }
};


#endif
