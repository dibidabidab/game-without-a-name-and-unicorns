
#ifndef GAME_PLANTENTITY_H
#define GAME_PLANTENTITY_H

#include "EntityTemplate.h"
#include "RopeEntity.h"

class PlantEntity : public EntityTemplate
{
public:
    entt::entity create() override
    {
        entt::entity ropeEntity = room->getTemplate<RopeEntity>()->create();

        entt::entity e = room->entities.create();

        setParent(ropeEntity, e, "rope");

        room->entities.assign<AABB>(e, ivec2(3));
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/plant"), mu::randomInt(0, 2));
        room->entities.assign<DynamicCollider>(e);

        VerletRope &rope = room->entities.get<VerletRope>(ropeEntity);
        rope.endPointEntity = e;
        rope.gravity = ivec2(0, mu::random(10, 30));
        rope.friction = mu::random(0.7, 0.99);
        rope.length = mu::random(5, 15);
        rope.nrOfPoints = rope.length;

        if (rope.length > 10.0f)
        {
            entt::entity leafs = createChild(ropeEntity, "leafs");
            room->entities.assign<AABB>(leafs, ivec2(3));
            room->entities.assign<AttachToRope>(leafs, ropeEntity, mu::random(0.5, 0.7));
            room->entities.assign<AsepriteView>(leafs, asset<aseprite::Sprite>("sprites/plant"), mu::randomInt(2, 4));
        }


        room->entities.assign<DrawPolyline>(ropeEntity, std::vector<uint8>{4u});

        return e;
    }
};


#endif
