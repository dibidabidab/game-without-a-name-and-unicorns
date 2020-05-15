
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
            const SpriteBobbing &bobbing, AsepriteView &view, const AABB &body
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

            float diffFract = float(diff) / view.sprite->height;

            view.originAlign.y = .5 - diffFract;
        });
    }
};


#endif
