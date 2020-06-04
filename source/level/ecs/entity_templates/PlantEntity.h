
#ifndef GAME_PLANTENTITY_H
#define GAME_PLANTENTITY_H

#include "EntityTemplate.h"
#include "RopeEntity.h"

class PlantEntity : public EntityTemplate
{
    // TODO: move to mu
    inline float remap (float value, float from1, float to1, float from2, float to2) {
        return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
    }

    const float LENGTH = 23.0f;

public:
    entt::entity create() override
    {
        entt::entity ropeEntity = room->getTemplate<RopeEntity>()->create();
        room->entities.get<VerletRope>(ropeEntity).nrOfPoints = 4;  // too high nr of points is costly and not needed for plants

        entt::entity e = room->entities.create();

        setParent(ropeEntity, e, "rope");

        room->entities.assign<AABB>(e, ivec2(3));
        room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/plant"), mu::randomInt(0, 3));
        room->entities.assign<DynamicCollider>(e);

        VerletRope &rope = room->entities.get<VerletRope>(ropeEntity);
        float length = mu::random(5, (mu::random() > 0.5f)? LENGTH : LENGTH * 0.5f);
        rope.endPointEntity = e;
        rope.gravity = ivec2(mu::random(-2, 2), mu::random(10, 30));
        rope.friction = mu::random(0.7, 0.99);
        rope.length = length;
        rope.nrOfPoints = min(length, 15.0f);

        while (length > 7.0f)
        {
            length -= mu::random(7, 10);

            entt::entity leafs = createChild(ropeEntity, "leafs");
            room->entities.assign<AABB>(leafs, ivec2(3));
            room->entities.assign<AttachToRope>(leafs, ropeEntity, 0.8f - (length / LENGTH));
            room->entities.assign<AsepriteView>(leafs, asset<aseprite::Sprite>("sprites/plant"), mu::randomInt(3, 5));
        }



        room->entities.assign<DrawPolyline>(ropeEntity, std::vector<uint8>{4u});

        return e;
    }
};


#endif
