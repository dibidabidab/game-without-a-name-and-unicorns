
#ifndef GAME_LEGSSYSTEM_H
#define GAME_LEGSSYSTEM_H

#include "../EntitySystem.h"
#include "../../../level/room/Room.h"
#include "../../components/body_parts/Leg.h"
#include "../../components/physics/Physics.h"

/**
 * Responsible for:
 * setting the position of feet based on the movement of an entity.
 */
class LegsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    constexpr static float MIN_JUMP_TIME = .1;

  protected:
    Room *room;

    void update(double deltaTime, EntityEngine *engine) override
    {
        this->room = (Room *) engine;
        room->entities.view<Leg, AABB>().each([&](auto e, Leg &leg, AABB &footAABB) {

            AABB *bodyAABB = room->entities.try_get<AABB>(leg.body);
            Physics *bodyPhysics = room->entities.try_get<Physics>(leg.body);
            if (!bodyAABB || !bodyPhysics) // leg is not attached to body.
            {
                // todo?
                return;
            }
            setTarget(leg, footAABB, *bodyAABB, *bodyPhysics);

            teleportFootIfNeeded(leg, footAABB);

            if (bodyPhysics->touches.floor && !bodyPhysics->prevTouched.floor && bodyPhysics->airTime > MIN_JUMP_TIME)
                leg.stopMoving();

            if (leg.moving)
                moveLeg(leg, footAABB, *bodyAABB, *bodyPhysics, deltaTime);
            else
            {
                startMovingIfNeeded(leg, footAABB, *bodyPhysics, deltaTime);
                footAABB.center += bodyPhysics->pixelsMovedByPolyPlatform;
            }

            leg.prevBodyPos = bodyAABB->center;
            leg.prevBodyVelocity = bodyPhysics->velocity;
        });
    }

    void teleportFootIfNeeded(Leg &leg, AABB &footAABB)
    {
        float dist = length(vec2(footAABB.center) - leg.target);
        if (!leg.initialized || dist > max<float>(32, leg.maxDistToTarget * 2.5))
        {
            footAABB.center = leg.target;
            leg.initialized = true;
        }
    }

    bool isJumping(const Physics &body)
    {
        return !body.touches.floor && (body.velocity.y > 0 || body.airTime > MIN_JUMP_TIME);
    }

    void setTarget(
            Leg &leg,
            const AABB &footAABB,
            const AABB &bodyAABB,
            const Physics &bodyPhysics
    )
    {
        TerrainCollisionDetector collisionDetector(room->getMap(), &room->entities);
        AABB tempFoot = footAABB;

        vec2 prevTarget = leg.target;

        if (!isJumping(bodyPhysics))
        {
            if (abs(bodyPhysics.velocity.x) < 10) // kinda standing still
            {
                for (int y = bodyPhysics.touches.polyPlatform ? -bodyAABB.halfSize.y : 0; y < 8; y++)
                {
                    leg.target = bodyAABB.center + leg.anchor;
                    leg.target.x += leg.idleXPos;
                    leg.target.y = bodyAABB.center.y - bodyAABB.halfSize.y - y;

                    tempFoot.center = leg.target;

                    TerrainCollisions footTouches;
                    collisionDetector.detect(footTouches, tempFoot, false, !bodyPhysics.touches.polyPlatform);

                    if (footTouches.pixelsAbovePolyPlatform != 0)
                    {
                        leg.target.y -= footTouches.pixelsAbovePolyPlatform;
                        break;
                    }
                    if (footTouches.floor)
                        break;
                }

                leg.maxDistToTarget = 2;
            }
            else    // walking
            {
                int dir = bodyPhysics.velocity.x < 0 ? -1 : 1;

                for (float angle = bodyPhysics.touches.polyPlatform ? -20 : 0; angle <= 90; angle++)
                {
                    leg.target = rotate(vec2(dir * leg.length, 0), dir * -angle * mu::DEGREES_TO_RAD);
                    leg.target.x *= .9;
                    leg.target += bodyAABB.center + leg.anchor;

                    tempFoot.center = leg.target;

                    TerrainCollisions footTouches;
                    collisionDetector.detect(footTouches, tempFoot, false, bodyPhysics.ignorePolyPlatforms);

                    if (footTouches.pixelsAbovePolyPlatform != 0 && footTouches.pixelsAbovePolyPlatform < 4)
                    {
                        leg.target.y -= footTouches.pixelsAbovePolyPlatform;
                        break;
                    }
                    if (footTouches.floor && !footTouches.ceiling)
                        break;
                }
                leg.maxDistToTarget = leg.stepSize;
            }
        }
        else // in air:
        {
            leg.target = bodyAABB.center + leg.anchor;
            leg.target.y = bodyAABB.bottomRight().y;

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

    void startMovingIfNeeded(Leg &leg, AABB &footAABB, const Physics &body, float deltaTime)
    {
        leg.shouldBeMoving = false;

        float dist = length(vec2(footAABB.center) - leg.target);

        bool justStartedWalking = leg.prevBodyVelocity.x == 0 && body.velocity.x != 0;

        bool justLanded = !body.prevTouched.floor && body.touches.floor && body.airTime > MIN_JUMP_TIME;

        if (dist < leg.maxDistToTarget && !justStartedWalking && !justLanded)
            return;

        // foot is too far away from target and should start moving.
        leg.shouldBeMoving = true;

        Leg *oppositeLeg = room->entities.try_get<Leg>(leg.oppositeLeg);
        if (oppositeLeg && oppositeLeg->moving && body.touches.floor)
        {   /**
             * the opposite leg is already 'walking'.
             * 3 things can happen now:
             * - let the opposite leg stop moving and let this leg start moving
             * - just start moving because distance to target is just too large
             * - let the opposite leg move faster, so this leg can start moving earlier
             */

            // option 1:
            if ((oppositeLeg->timeSinceStartedMoving == 0 || justLanded) && oppositeLeg->distToTargetBeforeMoving < dist)
                oppositeLeg->stopMoving();

            // option 2:
            else if (dist > leg.maxDistToTarget * 1.5)
            {} // do nothing. Leg will start moving.

            // option 3:
            else {
                oppositeLeg->oppositeLegWaiting += deltaTime;
                return; // return to make this leg wait.
            }
        }

        // start moving leg:
        leg.moving = true;
        leg.distToTargetBeforeMoving = dist;
    }

    void moveLeg(Leg &leg, AABB &footAABB, const AABB &bodyAABB, const Physics &bodyPhysics, float deltaTime)
    {
        leg.timeSinceStartedMoving += deltaTime;

        // first move foot the same amount as the body did:
        {
            ivec2 deltaPos = bodyAABB.center - leg.prevBodyPos;
            footAABB.center += deltaPos;
        }

        vec2 pixelsToMove = leg.moveAccumulator;

        if (!isJumping(bodyPhysics))
            pixelsToMove += moveFootWithArc(leg, footAABB, bodyAABB, bodyPhysics, deltaTime);
        else
            pixelsToMove += moveFootWithoutArc(leg, footAABB, bodyAABB, bodyPhysics, deltaTime);

        // store remains for next time:
        leg.moveAccumulator = pixelsToMove - vec2(ivec2(pixelsToMove));

        footAABB.center += pixelsToMove;
    }

    vec2 moveFootWithArc(Leg &leg, AABB &footAABB, const AABB &bodyAABB, const Physics &bodyPhysics, float deltaTime)
    {
        vec2 diff = leg.target - vec2(footAABB.center) - leg.moveAccumulator;
        float dist = length(diff);  // distance between foot and target

        if (dist < max(1., leg.distToTargetBeforeMoving * .3)) // foot is near target.
        {
            if (dist < 1) // foot is ~ 1 pixel away from target. Snap to target and stop moving:
            {
                leg.stopMoving();
                footAABB.center = leg.target;
                return vec2(0);
            }

            TerrainCollisionDetector collisionDetector(room->getMap());
            TerrainCollisions footTouches;
            collisionDetector.detect(footTouches, footAABB, false, bodyPhysics.touches.polyPlatform);

            if (footTouches.floor) // foot has reached the floor. stop moving
            {
                leg.stopMoving();
                return vec2(0);
            }
        }

        vec2 dir = diff / dist; // direction to target

        if (dist > 4) // rotate the direction 'dir' to make the foot move in an arc:
        {
            int walkDir = dir.x > 0 ? 1 : -1;

            float angle = walkDir * leg.stepArcAngle;

            // make the angle less big as the foot nears target:
            angle *= min(1.f, abs(diff.x) / leg.distToTargetBeforeMoving);

            dir = rotate(dir, angle * mu::DEGREES_TO_RAD);
        }

        float stepSpeed = leg.idleStepSpeed;
        float bodyVelX = abs(bodyPhysics.velocity.x);
        if (bodyVelX > 2) // body is not 'idle'
        {
            stepSpeed = bodyVelX * 1.8; // todo, dont hardcode

            if (bodyPhysics.touches.halfSlopeDown || bodyPhysics.touches.halfSlopeUp)
                stepSpeed *= 1.11803; // https://www.wolframalpha.com/input/?i=distance+between+%280%2C+0%29+and+%281%2C+0.5%29
            else if (bodyPhysics.touches.slopeDown || bodyPhysics.touches.slopeUp)
                stepSpeed *= 1.41421; // https://www.wolframalpha.com/input/?i=distance+between+%280%2C+0%29+and+%281%2C+1%29
        }

        if (leg.oppositeLegWaiting > 0)
        {
            // opposite leg is waiting.. increase stepSpeed:
            stepSpeed *= 1 + (leg.oppositeLegWaiting * 20.);
        }

        float stepDist = min(dist, stepSpeed * deltaTime);

        return dir * stepDist;
    }

    vec2 moveFootWithoutArc(Leg &leg, AABB &footAABB, const AABB &bodyAABB, const Physics &bodyPhysics, float deltaTime)
    {
        vec2 diff = leg.target - vec2(footAABB.center);
        float dist = length(diff);

        if (dist == 0)
            return vec2(0);

        if (dist < 2)
        {
            footAABB.center = leg.target;
            return vec2(0);
        }

        float stepDist = min(dist, leg.inAirStepSpeed * deltaTime);
        vec2 dir = diff / dist;

        return dir * stepDist;
    }

};


#endif
