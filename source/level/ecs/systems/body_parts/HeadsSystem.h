
#ifndef GAME_HEADSSYSTEM_H
#define GAME_HEADSSYSTEM_H

#include "../EntitySystem.h"
#include "../../../room/Room.h"
#include "../../components/body_parts/Head.h"
#include "../../components/physics/Physics.h"
#include "../../components/graphics/AsepriteView.h"
#include "../../components/PlatformerMovement.h"

class HeadsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    void update(double deltaTime, Room *room) override
    {
        TerrainCollisionDetector collisionDetector(room->getMap());

        room->entities.view<Head, AABB, AsepriteView>().each([&](Head &head, const AABB &aabb, AsepriteView &sprite) {

            for (int i = 0; i <= head.maxHeadRetraction; i++)
            {
                sprite.positionOffset.y = -i;

                AABB tmpAABB = aabb;
                tmpAABB.center.y -= i;

                if (!collisionDetector.detect(tmpAABB, true).ceiling)
                    break;
            }

            Flip *bodyFlip = room->entities.try_get<Flip>(head.body);
            if (bodyFlip && bodyFlip->horizontal != sprite.flipHorizontal)
            {
                head.timeSinceBodyFlipped += deltaTime;
                if (head.timeSinceBodyFlipped >= head.turnHeadDelay)
                {
                    sprite.flipHorizontal = bodyFlip->horizontal;
                    head.timeSinceBodyFlipped = 0;
                }
            }
        });
    }

};


#endif
