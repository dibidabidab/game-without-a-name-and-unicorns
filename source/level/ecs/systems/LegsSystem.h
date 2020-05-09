
#ifndef GAME_LEGSSYSTEM_H
#define GAME_LEGSSYSTEM_H

#include "EntitySystem.h"
#include "../../room/Room.h"
#include "../components/Leg.h"
#include "../components/Physics.h"


class LegsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void init(Room *room) override
    {
        updateFrequency = 60;
    }

    Room *room;

    void update(double deltaTime, Room *room) override
    {
        this->room = room;
        room->entities.view<Leg, AABB>().each([&](Leg &leg, AABB &footAABB) {

            AABB *anchorAABB = room->entities.try_get<AABB>(leg.anchor);
            AABB *bodyAABB = room->entities.try_get<AABB>(leg.body);
            Physics *bodyPhysics = room->entities.try_get<Physics>(leg.body);
            if (!anchorAABB || !bodyAABB || !bodyPhysics) // leg is not attached to body.
            {
                // todo?
                return;
            }
            setTarget(leg, footAABB, *anchorAABB, *bodyAABB, *bodyPhysics);

            if (leg.moving)
                moveLeg(leg, footAABB, *bodyPhysics, deltaTime);
            else
                startMovingIfNeeded(leg, footAABB, *anchorAABB);
        });
    }

    void setTarget(
            Leg &leg,
            const AABB &footAABB,
            const AABB &anchorAABB,
            const AABB &bodyAABB,
            const Physics &bodyPhysics
    )
    {
        TerrainCollisionDetector collisionDetector(room->getMap());
        AABB tempFoot = footAABB;

        vec2 prevTarget = leg.target;

        if (bodyPhysics.touches.floor)
        {
            if (bodyPhysics.velocity.x == 0) // standing still
            {
                for (int y = 0; y < 10; y++)
                {
                    leg.target = anchorAABB.center;
                    leg.target.x += leg.idleXPos;
                    leg.target.y = bodyAABB.center.y - bodyAABB.halfSize.y - y;

                    tempFoot.center = leg.target;

                    auto footTouches = collisionDetector.detect(tempFoot, false);
                    if (footTouches.floor)
                        break;
                }

                leg.maxDistToTarget = 0;
            }
            else    // walking
            {
                int dir = bodyPhysics.velocity.x < 0 ? -1 : 1;

                for (float angle = 10; angle <= 90; angle += 3)
                {
                    leg.target = rotate(vec2(dir * leg.length, 0), dir * -angle * mu::DEGREES_TO_RAD);
                    leg.target += anchorAABB.center;

                    tempFoot.center = leg.target;

                    auto footTouches = collisionDetector.detect(tempFoot, false);

                    leg.target.y -= footAABB.halfSize.y * 2;

                    if (footTouches.floor)
                        break;
                }
                leg.maxDistToTarget = leg.stepSize;
            }
        }
        leg.target *= .5;
        leg.target += prevTarget * .5f;
    }

    void startMovingIfNeeded(Leg &leg, const AABB &footAABB, const AABB &anchorAABB)
    {
        float distToTarget = length(leg.target - vec2(footAABB.center));
        if (distToTarget > leg.maxDistToTarget)
        {
            Leg *opposite = room->entities.try_get<Leg>(leg.oppositeLeg);
            if (opposite && opposite->moving

                && (distToTarget < leg.maxDistToTarget * 1.3 || leg.maxDistToTarget == 0)
            )
            {
                if (opposite->distToTargetBeforeMoving < distToTarget && leg.timeSinceStartedMoving == 0.)
                {
                    opposite->moving = false;
                }
                else return;
            }

            leg.timeSinceStartedMoving = 0;
            leg.moving = true;
            leg.distToTargetBeforeMoving = distToTarget;
        }
    }

    void moveLeg(Leg &leg, AABB &footAABB, const Physics &bodyPhysics, float deltaTime)
    {
        vec2 diff = leg.target - vec2(footAABB.center);
        float dist = length(diff);

        if (dist < leg.distToTargetBeforeMoving * .2)
        {
            TerrainCollisionDetector collisionDetector(room->getMap());
            AABB largerFootAABB = footAABB;
            largerFootAABB.halfSize.y++;

            if ((bodyPhysics.touches.floor && dist < 1.5) || collisionDetector.detect(largerFootAABB, false).floor) // foot has reached the floor. stop moving
            {
                leg.moving = false;
                return;
            }
        }

        vec2 dir = diff / dist;

        float step = min<float>(dist, max<float>(length(bodyPhysics.velocity), leg.idleStepSpeed) * deltaTime * 2);

        dir = rotate(dir, (dir.x > 0 ? 1 : -1) * 60 * min<float>(1, abs(diff.x) / leg.distToTargetBeforeMoving) * mu::DEGREES_TO_RAD);

        vec2 move = dir * step + (bodyPhysics.velocity * deltaTime) + leg.moveAccumulator;
        ivec2 moveInt = move;

        leg.moveAccumulator = move - vec2(moveInt);

        footAABB.center += moveInt;
        leg.timeSinceStartedMoving += deltaTime;
    }

};


#endif
