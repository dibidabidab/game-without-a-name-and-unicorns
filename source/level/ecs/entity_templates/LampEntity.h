
#ifndef GAME_LAMPENTITY_H
#define GAME_LAMPENTITY_H

#include "EntityTemplate.h"
#include "../components/physics/Physics.h"
#include "../components/graphics/Light.h"
#include "RopeEntity.h"
#include "../components/graphics/AsepriteView.h"
#include "../components/graphics/DrawPolyline.h"

class LampEntity : public EntityTemplate
{
  public:
    entt::entity create() override
    {
        entt::entity ropeEntity = room->getTemplate<RopeEntity>()->create();

        entt::entity e = room->entities.create();

        setParent(ropeEntity, e, "rope");

        room->entities.assign<AABB>(e, ivec2(3));
        room->entities.assign<LightPoint>(e);
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/lamp"));

        room->entities.assign<DynamicCollider>(e);

        room->entities.get<VerletRope>(ropeEntity).endPointEntity = e;
        room->entities.assign<DrawPolyline>(ropeEntity, std::vector<uint8>{4u});

        return e;
    }
};


#endif
