
#ifndef GAME_SPRITEBOBBINGSYSTEM_H
#define GAME_SPRITEBOBBINGSYSTEM_H

#include "../../room/Room.h"
#include "EntitySystem.h"
#include "../components/SpriteBobbing.h"
#include "../components/AsepriteView.h"
#include "../components/Physics.h"

class SpriteBobbingSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<SpriteBobbing, AsepriteView, AABB>().each([&](
            auto e, SpriteBobbing &bobbing, AsepriteView &view, const AABB &body
        ) {

            float footHeight = 0;
            int n = 0;
            for (auto e : bobbing.feet)
            {
                const AABB *footAABB = room->entities.try_get<AABB>(e);
                if (footAABB)
                {
                    n++;
                    footHeight += footAABB->center.y;
                }
            }
            int avgFootHeight = round(footHeight / n);
            int diff = avgFootHeight - body.center.y + body.halfSize.y;

            Physics *physics = room->entities.try_get<Physics>(e);
            if (physics)
            {
                if (physics->touches.floor && !physics->prevTouched.floor)
                    bobbing.floorHitVelocity = physics->prevVelocity.y * .25;
                if (!physics->touches.floor && bobbing.floorHitVelocity < 0)
                    bobbing.floorHitVelocity = 0;

                bobbing.floorHitVelocity += 250 * deltaTime;
                bobbing.floorHitYPos = min<float>(0, bobbing.floorHitYPos + bobbing.floorHitVelocity * deltaTime);

                diff += bobbing.floorHitYPos;
            }
            if (diff < bobbing.minYPos)
            {
                bobbing.floorHitVelocity = max(bobbing.floorHitVelocity, 0.f);
                diff = bobbing.minYPos;
            }
            if (diff > bobbing.maxYPos)
                diff = bobbing.maxYPos;

            float diffFract = float(diff) / view.sprite->height;

            view.originAlign.y = .5 - diffFract;
        });
    }
};


#endif
