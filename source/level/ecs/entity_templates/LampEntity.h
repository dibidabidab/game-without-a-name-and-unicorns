
#ifndef GAME_LAMPENTITY_H
#define GAME_LAMPENTITY_H

#include "EntityTemplate.h"
#include "../components/Physics.h"
#include "../components/Light.h"
#include "RopeEntity.h"

class LampEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity rope = room->getTemplate<RopeEntity>()->create();

        entt::entity e = room->entities.create();
        room->entities.assign<AABB>(e, ivec2(3));
        room->entities.assign<LightPoint>(e);
        room->entities.assign<AttachToRope>(e, EntityReference{room, rope});

        return e;
    }
};


#endif
