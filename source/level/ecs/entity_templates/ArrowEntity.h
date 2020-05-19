
#ifndef GAME_ARROWENTITY_H
#define GAME_ARROWENTITY_H

#include "EntityTemplate.h"
#include "../components/physics/Physics.h"
#include "../components/graphics/AsepriteView.h"
#include "../components/combat/Arrow.h"

class ArrowEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity e = room->entities.create();

        room->entities.assign<AABB>(e, ivec2(1));
        room->entities.assign<Physics>(e, 400.f, vec2(0), true);
        room->entities.assign<StaticCollider>(e);
        room->entities.assign<Arrow>(e);
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/arrow"));

        return e;
    }
};


#endif
