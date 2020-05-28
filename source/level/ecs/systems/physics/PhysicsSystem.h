
#ifndef GAME_PHYSICSSYSTEM_H
#define GAME_PHYSICSSYSTEM_H


#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/physics/Physics.h"
#include "../../components/physics/PolyPlatform.h"
#include "../../components/Polyline.h"

namespace
{
    enum Move
    {
        up, down, left, right, none
    };
}

/**
 * Responsible for moving an entity based on:
 *  - velocity
 *  - gravity
 *  - terrain collisions
 *  - collisions with StaticColliders
 *  - distance constraints
 */
class PhysicsSystem : public EntitySystem
{
    Room *room = NULL;

    using EntitySystem::EntitySystem;
  protected:
    void update(double deltaTime, Room *room) override
    {
        this->room = room;
        collisionDetector = new TerrainCollisionDetector(room->getMap(), &room->entities);

        room->entities.view<Physics, AABB>().each([&](Physics &physics, AABB &body) {                 /// PHYSICS UPDATE

            TerrainCollisions tmp = physics.touches;
            auto tmpVel = physics.velocity;

            updatePosition(physics, body, deltaTime);
            updateVelocity(physics, deltaTime);

            physics.prevTouched = tmp;
            if (!physics.touches.floor)
                physics.airTime += deltaTime;
            if (physics.prevTouched.floor && !physics.touches.floor)
                physics.airTime = 0;

            physics.prevVelocity = tmpVel;
        });


        auto staticColliders = room->entities.view<AABB, StaticCollider>();             /// DYNAMIC COLLIDES WITH STATIC

        room->entities.view<AABB, DynamicCollider>().each([&](auto e, AABB &dynamicAABB, DynamicCollider &dynCol) {

            staticColliders.each([&](const AABB &staticAABB, auto) {

                if (!dynamicAABB.overlaps(staticAABB))
                    return;

                // todo: emit collision event

                if (dynCol.repositionAfterCollision)
                {
                    repositionAfterCollision(staticAABB, dynamicAABB, e);

                    DistanceConstraint *constraint = room->entities.try_get<DistanceConstraint>(e);
                    if (constraint) for (int i = 0; i < 20; i++)
                    {
                        auto posBefore = dynamicAABB.center;

                        updateDistanceConstraint(dynamicAABB, *constraint);

                        if (dynamicAABB.center == posBefore)
                            break;

                        repositionAfterCollision(staticAABB, dynamicAABB, e);
                    }
                }
            });
        });

        room->entities.view<AABB, DistanceConstraint>().each([&](AABB &aabb, const DistanceConstraint &constraint) {
            updateDistanceConstraint(aabb, constraint);
        });

        room->entities.view<AABB, PolyPlatform, Polyline>().each([&](AABB &aabb, PolyPlatform &platform, const Polyline &line) {

            aabb.halfSize = ivec2(0);
            for (auto &p : line.points)
            {
                aabb.halfSize.x = std::max<int>(aabb.halfSize.x, abs(p.x));
                aabb.halfSize.y = std::max<int>(aabb.halfSize.y, abs(p.y));
            }
        });

        delete collisionDetector;
    }

  private:

    void updateDistanceConstraint(AABB &aabb, const DistanceConstraint &constraint)
    {
        const AABB *targetAABB = room->entities.try_get<AABB>(constraint.target);
        if (!targetAABB)
            return;

        vec2 target = vec2(targetAABB->center) + constraint.targetOffset;

        vec2 diff = vec2(aabb.center) - target;
        float dist = length(diff);
        if (dist > constraint.maxDistance)
        {
            vec2 dir = diff / dist;

            aabb.center = target + dir * constraint.maxDistance;
        }
    }

    void repositionAfterCollision(const AABB &staticAABB, AABB &dynAABB, entt::entity dynEntity)
    {
        ivec2
            diff = abs(staticAABB.center - dynAABB.center),

            overlap = staticAABB.halfSize + dynAABB.halfSize - diff;

        int
            axis = overlap.x > overlap.y ? 1 : 0,

            direction = dynAABB.center[axis] < staticAABB.center[axis] ? -1 : 1;

        ivec2 pixelsToMove(0);
        pixelsToMove[axis] = overlap[axis] * direction;

        Physics *p = room->entities.try_get<Physics>(dynEntity);
        if (!p)
        {
            dynAABB.center += pixelsToMove;
            return;
        }

        if (p->velocity[axis] * direction < 0) // if the entity wants to overlap even more -> reset velocity.
            p->velocity[axis] = 0;

        moveBody(*p, dynAABB, pixelsToMove);
    }

    TerrainCollisionDetector *collisionDetector;

    /**
     * Updates the velocity of a body.
     */
    void updateVelocity(Physics &physics, double deltaTime)
    {
        physics.velocity.y -= physics.gravity * deltaTime;

        float friction = physics.airFriction;

        if (physics.touches.floor)
            friction = physics.floorFriction;
        else if (physics.velocity.y < 0 && (physics.touches.rightWall || physics.touches.leftWall))
            friction = physics.wallFriction;

        friction *= deltaTime;

        physics.velocity *= max(0.f, 1.f - friction);

        if (physics.touches.floor && physics.velocity.y < 0) physics.velocity.y = 0;
        if (physics.touches.ceiling && physics.velocity.y > 0) physics.velocity.y = 0;

        if (physics.touches.leftWall && physics.velocity.x < 0) physics.velocity.x = 0;
        if (physics.touches.rightWall && physics.velocity.x > 0) physics.velocity.x = 0;
    }

    /**
     * Updates the position of a body using its velocity
     */
    void updatePosition(Physics &physics, AABB &body, double deltaTime)
    {
        vec2 pixelsToMove = vec2(physics.velocity) * vec2(deltaTime);
        pixelsToMove += physics.velocityAccumulator; // add remains of previous update.

        vec2 temp = pixelsToMove;

        moveBody(physics, body, pixelsToMove);
        physics.velocityAccumulator = pixelsToMove; // store remains for next update

        if (temp == pixelsToMove) // if not moved
            updateTerrainCollisions(physics, body); // terrain might have changed
    }

    template <typename vec>
    void moveBody(Physics &physics, AABB &body, vec &pixelsToMove)
    {
        while (true)
        {
            Move toDo;
            if (pixelsToMove.y >= 1)         toDo = up;     // prioritize vertical movement to prevent entity from walking through a sloped polyPlatform.
            else if (pixelsToMove.y <= -1)   toDo = down;
            else if (pixelsToMove.x >= 1)    toDo = right;
            else if (pixelsToMove.x <= -1)   toDo = left;
            else break;

            // if entity is on a polyPlatform that has a different height at the next x position:
            if (toDo == right && physics.touches.polyPlatformDeltaRight != 0)
            {
                if (pixelsToMove.y > 0)
                    pixelsToMove.y = max<float>(pixelsToMove.y, physics.touches.polyPlatformDeltaRight);
                else
                    pixelsToMove.y = physics.touches.polyPlatformDeltaRight;
            }
            if (toDo == left && physics.touches.polyPlatformDeltaLeft != 0)
            {
                if (pixelsToMove.y > 0)
                    pixelsToMove.y = max<float>(pixelsToMove.y, physics.touches.polyPlatformDeltaLeft);
                else
                    pixelsToMove.y = physics.touches.polyPlatformDeltaLeft;
            }

            ivec2 prevPos = body.center;

            if (tryMove(physics, body, toDo)) // move succeeded -> decrease pixelsToMove:
            {
                switch (toDo)
                {
                    case up:    pixelsToMove.y--;
                        break;
                    case down:  pixelsToMove.y++;
                        break;
                    case left:  pixelsToMove.x++;
                        break;
                    case right: pixelsToMove.x--;
                    case none:
                        break;
                }
            } // move is not possible:
            else if (toDo == Move::left || toDo == Move::right)  pixelsToMove.x = 0; // cant move horizontally anymore.
            else                                                 pixelsToMove.y = 0; // cant move vertically anymore.
        }
    }

    /**
     * Will try to do the move, returns true if success
     */
    bool tryMove(Physics &physics, AABB &body, Move toDo)
    {
        std::list<Move> movesToDo;

        Move prevMove = Move::none;
        while (toDo != Move::none)
        {
            movesToDo.push_back(toDo);

            if (!canDoMove(physics, body, toDo, prevMove))
                return false;
            prevMove = movesToDo.back();
        }
        for (auto move : movesToDo)
            doMove(physics, body, move);
        updateTerrainCollisions(physics, body);
        return true;
    }

    /**
     * Checks if a move is possible.
     * Sometimes a move is possible but requires another move to be done as well,
     * therefore `moveToDo` will be changed to that other move (or Move::none)
     */
    bool canDoMove(Physics &p, AABB &aabb, Move &moveToDo, Move prevMove)
    {
        Move originalMove = moveToDo;
        moveToDo = Move::none;

        switch (originalMove)
        {
            case up:    return !p.touches.ceiling;
            case down:
                return !p.touches.floor
                    || (p.touches.slopeDown && prevMove == right)
                    || (p.touches.slopeUp && prevMove == left);
            case left:
                if (p.touches.slopeDown && (!p.touches.halfSlopeDown || (aabb.bottomLeft().x + 1) % 2 == 0))
                    moveToDo = up;
                else if (p.touches.slopeUp && !p.touches.flatFloor && (!p.touches.halfSlopeUp || aabb.bottomRight().x % 2 == 0))
                    moveToDo = down;
                return !p.touches.leftWall;

            case right:
                if (p.touches.slopeUp && (!p.touches.halfSlopeUp || aabb.center.x % 2 == 0))
                    moveToDo = up;
                else if (p.touches.slopeDown && !p.touches.flatFloor && (!p.touches.halfSlopeDown || aabb.bottomLeft().x % 2 == 0))
                    moveToDo = down;
                return !p.touches.rightWall;

            case none:  return true;
        }
        return true;
    }

    /**
     * Executes the move (without checking if the move is possible to do)
     */
    void doMove(Physics &p, AABB &body, Move move)
    {
        switch (move)
        {
            case up:    body.center.y++; break;
            case down:  body.center.y--; break;
            case left:  body.center.x--; break;
            case right: body.center.x++; break;
            case none:                   break;
        }
    }

    void updateTerrainCollisions(Physics &p, AABB &body)
    {
        AABB outlineBox = body;
        outlineBox.halfSize += 1; // make box 1 pixel larger to detect if p.body *touches* terrain
        p.touches = collisionDetector->detect(outlineBox, p.ignorePlatforms, p.ignorePolyPlatforms);
    }

};

#endif
