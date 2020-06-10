
#ifndef GAME_RAINBOWSYSTEM_H
#define GAME_RAINBOWSYSTEM_H

#include "EntitySystem.h"
#include "../../room/Room.h"
#include "../components/Rainbow.h"
#include "../components/physics/Physics.h"
#include "../components/Polyline.h"

class RainbowSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override
    {

        room->entities.view<RainbowSpawner, AABB>().each([&](RainbowSpawner &spawner, const AABB &aabb) {

            if (spawner.rainbowEntity == entt::null)
                spawner.rainbowEntity = room->getTemplate("Rainbow").create();

            Polyline *rainbowLine = room->entities.try_get<Polyline>(spawner.rainbowEntity);
            AABB *rainbowAABB = room->entities.try_get<AABB>(spawner.rainbowEntity);
            if (!rainbowLine || !rainbowAABB)
                return;

            if (rainbowLine->points.empty())
            {
                ivec2 offset(spawner.maxRainbowLength * .5);
                rainbowAABB->center = aabb.center + offset;
            }
            ivec2 currPos = aabb.center - rainbowAABB->center;

            if (rainbowLine->points.empty())
            {
                rainbowLine->points.emplace_back(currPos);
                rainbowLine->points.emplace_back(currPos);
                spawner.prevPoint = currPos;
            }


            bool front = currPos.x < spawner.prevPoint.x;

            auto toMove = front ? rainbowLine->points.front() : rainbowLine->points.back();

            toMove = currPos;

            if (length(toMove - spawner.prevPoint) > 10)
            {
                if (front)
                    rainbowLine->points.emplace_front(currPos);
                else
                    rainbowLine->points.emplace_back(currPos);
                spawner.prevPoint = currPos;
            }
        });
    }
};


#endif
