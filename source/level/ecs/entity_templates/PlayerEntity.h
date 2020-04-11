
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
        static std::shared_ptr<NetworkedDataList> toSend;
        if (!toSend)
        {
            toSend = std::make_shared<NetworkedDataList>();
            toSend->components<PlatformerMovement, PlayerControlled>();
            toSend->componentGroup<Physics, AABB>(); // if any changes -> send all
        }
        networked.toSend = toSend;
    }

    void makeNetworkedClientSide(Networked &networked) override
    {
        static std::shared_ptr<NetworkedDataList> toReceive;
        if (!toReceive)
        {
            toReceive = std::make_shared<NetworkedDataList>();
            toReceive->components<PlatformerMovement, PlayerControlled>();
            toReceive->componentGroup<Physics, AABB>(); // if any changes -> send all
        }
        networked.toReceive = toReceive;
    }
};


#endif
