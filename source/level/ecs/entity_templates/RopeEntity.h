
#ifndef GAME_ROPEENTITY_H
#define GAME_ROPEENTITY_H

#include "EntityTemplate.h"
#include "../components/physics/Physics.h"
#include "../components/physics/VerletRope.h"

class RopeEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<AABB>(e, ivec2(4), ivec2(256));
        room->entities.assign<VerletRope>(e);

        return e;
    }
};


#endif
