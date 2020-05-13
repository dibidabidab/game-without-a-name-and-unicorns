
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
    Room *room;

    void update(double deltaTime, Room *room) override
    {
        this->room = room;
        room->entities.view<Leg, AABB>().each([&](auto e, Leg &leg, AABB &footAABB) {

            AABB *bodyAABB = room->entities.try_get<AABB>(leg.body);
            Physics *bodyPhysics = room->entities.try_get<Physics>(leg.body);
            if (!bodyAABB || !bodyPhysics) // leg is not attached to body.
            {
                // todo?
                return;
            }
            setTarget(leg, footAABB, *bodyAABB, *bodyPhysics);

            if (leg.moving)
                moveLeg(leg, footAABB, *bodyAABB, *bodyPhysics, deltaTime);
            else
                startMovingIfNeeded(leg, footAABB, *bodyPhysics);
        });
    }

    void setTarget(
            Leg &leg,
            const AABB &footAABB,
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
                for (int y = 0; y < 4; y++)
                {
                    leg.target = bodyAABB.center + leg.anchor;
                    leg.target.x += leg.idleXPos;
                    leg.target.y = bodyAABB.center.y - bodyAABB.halfSize.y - y;

                    tempFoot.center = leg.target;

                    auto footTouches = collisionDetector.detect(tempFoot, false);
                    if (footTouches.floor)
                        break;
                }

                leg.maxDistToTarget = 3;
            }
            else    // walking
            {
                int dir = bodyPhysics.velocity.x < 0 ? -1 : 1;

                for (float angle = 10; angle <= 90; angle += 2)
                {
                    leg.target = bodyAABB.center + leg.anchor;
                    leg.target += rotate(vec2(dir * leg.length, 0), dir * -angle * mu::DEGREES_TO_RAD);

                    tempFoot.center = leg.target;

                    auto footTouches = collisionDetector.detect(tempFoot, false);

                    leg.target.y -= footAABB.halfSize.y * 2;

                    if (footTouches.floor && !footTouches.ceiling)
                        break;
                }
                leg.maxDistToTarget = leg.stepSize;
            }
        }
        else // in air:
        {
            leg.target = bodyAABB.center + leg.anchor;
            leg.target.y -= leg.length;

            int dir = bodyPhysics.velocity.x < 0 ? -1 : 1;
            bool retract = leg.anchor.x * dir < 0;
            if (bodyPhysics.velocity.y > 0)
                retract = !retract;

            if (retract)
                leg.target.y += leg.length * .3;

            if (!retract && bodyPhysics.velocity.x != 0)
            {
                leg.target.x += dir * leg.length * .2 * (bodyPhysics.velocity.y < 0 ? 1 : -1);
            }

            leg.maxDistToTarget = 0;
        }
    }

    void startMovingIfNeeded(Leg &leg, const AABB &footAABB, const Physics &body)
    {
        float distToTarget = length(leg.target - vec2(footAABB.center));
        if (distToTarget > leg.maxDistToTarget) // foot is too far away from target.
        {
            Leg *opposite = room->entities.try_get<Leg>(leg.oppositeLeg);

            // check if opposite leg is currently moving. If so, this leg might have to wait.
            if (body.touches.floor && opposite && opposite->moving

                && (distToTarget < leg.maxDistToTarget * 1.5 || leg.maxDistToTarget == 0)
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

    void moveLeg(Leg &leg, AABB &footAABB, const AABB &bodyAABB, const Physics &bodyPhysics, float deltaTime)
    {
        vec2 move(0);
        if (bodyPhysics.touches.floor)
        {
            if (!bodyPhysics.prevTouched.floor)
            {
                leg.moving = false;
                return;
            }

            // step with arc
            move = moveFootWithArc(leg, footAABB, bodyAABB, bodyPhysics, deltaTime);
        }
        else // in air.
        {
            // just move foot towards target
            vec2 dir = leg.target - vec2(footAABB.center);
            float dist = length(dir);
            if (dist != 0)
            {
                dir /= dist;

                float speed = 30;
                move = dir * speed * deltaTime;
                move += bodyPhysics.velocity * deltaTime;
            }
            move += leg.moveAccumulator;
        }
        ivec2 moveInt = move;
        // store remains for next time:
        leg.moveAccumulator = move - vec2(moveInt);

        footAABB.center += moveInt;

        // limit y pos of foot:
        footAABB.center.y = min(footAABB.center.y, bodyAABB.center.y + leg.anchor.y);

        leg.timeSinceStartedMoving += deltaTime;
    }

    vec2 moveFootWithArc(Leg &leg, AABB &footAABB, const AABB &bodyAABB, const Physics &bodyPhysics, float deltaTime)
    {
        vec2 diff = leg.target - vec2(footAABB.center);
        float dist = length(diff);

        if (dist < max(1., leg.distToTargetBeforeMoving * .3)) // foot is near target.
        {
            if (dist < 1) // foot is ~ 1 pixel away from target. Snap to target and stop moving:
            {
                leg.moving = false;
                footAABB.center = leg.target;
                return vec2(0);
            }

            TerrainCollisionDetector collisionDetector(room->getMap());
            AABB largerFootAABB = footAABB;
            largerFootAABB.halfSize.y++;

            if (collisionDetector.detect(largerFootAABB, false).floor) // foot has reached the floor. stop moving
            {
                leg.moving = false;
                return vec2(0);
            }
        }

        vec2 dir = diff / dist;

        float stepSpeed = bodyPhysics.velocity.x == 0 ? leg.idleStepSpeed : abs(bodyPhysics.velocity.x);
        if (bodyPhysics.touches.slopeUp || bodyPhysics.touches.slopeDown)
            stepSpeed *= 1.4;

        float step = min<float>(dist, stepSpeed * deltaTime * 1.8);

        if (dist > 4) // rotate direction so that foot moves in arc:
            dir = rotate(dir, (dir.x > 0 ? 1 : -1) * leg.stepArcAngle * min<float>(1, abs(diff.x) / leg.distToTargetBeforeMoving) * mu::DEGREES_TO_RAD);

        // amount to move:
        vec2 move = dir * step + (bodyPhysics.velocity * deltaTime) + leg.moveAccumulator;
        return move;
    }

};


#endif
