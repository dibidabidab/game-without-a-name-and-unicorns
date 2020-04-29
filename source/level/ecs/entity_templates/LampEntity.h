
#ifndef GAME_LAMPENTITY_H
#define GAME_LAMPENTITY_H

#include "EntityTemplate.h"
#include "../components/Physics.h"
#include "../components/Light.h"

class LampEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();
        room->entities.assign<AABB>(e, ivec2(3), ivec2(32));
        room->entities.assign<LightPoint>(e);
        return e;
    }
};


#endif
