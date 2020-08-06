
#include "PhysicsSystem.h"

void PhysicsSystem::update(double deltaTime, Room *room)
{
    this->room = room;
    collisionDetector = new TerrainCollisionDetector(room->getMap(), &room->entities);

    room->entities.view<AABB, PolyPlatform, Polyline>().each([&](AABB &aabb, PolyPlatform &platform, const Polyline &line) {

        aabb.halfSize = ivec2(0, TerrainCollisionDetector::DETECT_POLY_PLATFORM_MARGIN);
        for (auto &p : line.points)
        {
            aabb.halfSize.x = std::max<int>(aabb.halfSize.x, abs(p.x));
            aabb.halfSize.y = std::max<int>(
                    aabb.halfSize.y,    // VV add some extra space so the CollisionDetector can detect PolyPlatforms below the entity
                    p.y > 0 ? p.y + TerrainCollisionDetector::DETECT_POLY_PLATFORM_MARGIN : -p.y
            );
        }

        if (platform.prevAABBPos != aabb.center)
        {
            ivec2 diff = aabb.center - platform.prevAABBPos;

            moveEntitiesOnPolyPlatform(platform, diff);

            platform.prevAABBPos = aabb.center;
        }

        platform.entitiesAbovePlatform.clear();
        platform.entitiesOnPlatform.clear();
    });


    room->entities.view<Physics, AABB>().each([&](auto e, Physics &physics, AABB &body) {                 /// PHYSICS UPDATE

        TerrainCollisions prevTouched = physics.touches;
        auto tmpVel = physics.velocity;

        updatePosition(physics, body, deltaTime);
        updateVelocity(physics, deltaTime);

        physics.prevTouched = prevTouched;

        preventFallingThroughPolyPlatform(physics, body);

        if (!physics.touches.floor)
            physics.airTime += deltaTime;
        if (physics.prevTouched.floor && !physics.touches.floor)
            physics.airTime = 0;

        physics.prevVelocity = tmpVel;

        if (physics.touches.abovePolyPlatformEntity != entt::null)
        {
            PolyPlatform &platform = room->entities.get<PolyPlatform>(physics.touches.abovePolyPlatformEntity);
            platform.entitiesAbovePlatform.push_back(e);
        }
        if (physics.touches.polyPlatform)
        {
            PolyPlatform &platform = room->entities.get<PolyPlatform>(physics.touches.polyPlatformEntity);
            platform.entitiesOnPlatform.push_back(e);
        }
        if (physics.justMovedByPolyPlatform)
            physics.justMovedByPolyPlatform = false;
        else
            physics.pixelsMovedByPolyPlatform = ivec2(0);
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

    delete collisionDetector;
}

void PhysicsSystem::moveEntitiesOnPolyPlatform(const PolyPlatform &platform, const ivec2 &diff)
{
    for (auto e : platform.entitiesOnPlatform)
    {
        AABB *aabbOnPlatform = room->entities.try_get<AABB>(e);
        Physics *physicsOnPlatform = room->entities.try_get<Physics>(e);
        if (!aabbOnPlatform || !physicsOnPlatform)
            continue;

        aabbOnPlatform->center += diff;
        physicsOnPlatform->pixelsMovedByPolyPlatform = diff;
        physicsOnPlatform->justMovedByPolyPlatform = true;
    }
}

void PhysicsSystem::updateDistanceConstraint(AABB &aabb, const DistanceConstraint &constraint)
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

void PhysicsSystem::repositionAfterCollision(const AABB &staticAABB, AABB &dynAABB, entt::entity dynEntity)
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

void PhysicsSystem::updateVelocity(Physics &physics, double deltaTime)
{
    physics.velocity.y -= physics.gravity * deltaTime;

    float friction = physics.airFriction;

    if (physics.touches.floor)
        friction = physics.floorFriction * room->getMap().getMaterialProperties(physics.touches.floorMaterial).friction;
    else if (physics.velocity.y < 0 && (physics.touches.rightWall || physics.touches.leftWall))
        friction = physics.wallFriction;

    friction *= deltaTime;

    physics.velocity *= max(0.f, 1.f - friction);

    if (!physics.ghost)
    {
        if (physics.touches.floor && physics.velocity.y < 0)
            physics.velocity.y *= -room->getMap().getMaterialProperties(physics.touches.floorMaterial).bounciness;

        if (physics.touches.ceiling && physics.velocity.y > 0) physics.velocity.y = 0;

        if (physics.touches.leftWall && physics.velocity.x < 0) physics.velocity.x = 0;
        if (physics.touches.rightWall && physics.velocity.x > 0) physics.velocity.x = 0;
    }
}

void PhysicsSystem::updatePosition(Physics &physics, AABB &body, double deltaTime)
{
    vec2 pixelsToMove = vec2(physics.velocity) * vec2(deltaTime);
    pixelsToMove += physics.velocityAccumulator; // add remains of previous update.

    vec2 temp = pixelsToMove;

    moveBody(physics, body, pixelsToMove);
    physics.velocityAccumulator = pixelsToMove; // store remains for next update

    if (temp == pixelsToMove) // if not moved
        updateTerrainCollisions(physics, body); // terrain might have changed
}

bool PhysicsSystem::tryMove(Physics &physics, AABB &body, Move toDo)
{
    std::list<Move> movesToDo; // doubly linked list

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

bool PhysicsSystem::canDoMove(Physics &physics, AABB &aabb, Move &moveToDo, Move prevMove)
{
    Move originalMove = moveToDo;
    moveToDo = Move::none;

    switch (originalMove)
    {
        case up:    return !physics.touches.ceiling;
        case down:
            return !physics.touches.floor
                   || (physics.touches.slopeDown && prevMove == right)
                   || (physics.touches.slopeUp && prevMove == left);
        case left:
            if (physics.touches.slopeDown && (!physics.touches.halfSlopeDown || (aabb.bottomLeft().x + 1) % 2 == 0))
                moveToDo = up;
            else if (physics.touches.slopeUp && !physics.touches.flatFloor && (!physics.touches.halfSlopeUp || aabb.bottomRight().x % 2 == 0))
                moveToDo = down;
            return !physics.touches.leftWall;

        case right:
            if (physics.touches.slopeUp && (!physics.touches.halfSlopeUp || aabb.center.x % 2 == 0))
                moveToDo = up;
            else if (physics.touches.slopeDown && !physics.touches.flatFloor && (!physics.touches.halfSlopeDown || aabb.bottomLeft().x % 2 == 0))
                moveToDo = down;
            return !physics.touches.rightWall;

        case none:  return true;
    }
    return true;
}

void PhysicsSystem::doMove(Physics &physics, AABB &body, Move move)
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

void PhysicsSystem::updateTerrainCollisions(Physics &physics, AABB &body)
{
    AABB outlineBox = body;
    outlineBox.halfSize += 1; // make box 1 pixel larger to detect if p.body *touches* terrain
    physics.touches = collisionDetector->detect(outlineBox, physics.ignorePlatforms, physics.ignorePolyPlatforms);
}

template<typename vec>
void PhysicsSystem::moveBody(Physics &physics, AABB &body, vec &pixelsToMove)
{
    if (physics.ghost)
    {
        body.center += pixelsToMove;
        pixelsToMove -= pixelsToMove;
        updateTerrainCollisions(physics, body);
        return;
    }
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

        if (tryMove(physics, body, toDo)) // move succeeded -> decrease pixelsToMove:
        {
            switch (toDo)
            {
                case up:    pixelsToMove.y--; break;
                case down:  pixelsToMove.y++; break;
                case left:  pixelsToMove.x++; break;
                case right: pixelsToMove.x--; break;
                case none: break;
            }
        } // move is not possible:
        else if (toDo == Move::left || toDo == Move::right)  pixelsToMove.x = 0; // cant move horizontally anymore.
        else                                                 pixelsToMove.y = 0; // cant move vertically anymore.
    }
}

void PhysicsSystem::preventFallingThroughPolyPlatform(Physics &physics, AABB &aabb)
{
    bool wasAbovePlatform =
            physics.prevTouched.abovePolyPlatformEntity != entt::null
            && !physics.touches.polyPlatform
            && physics.prevTouched.abovePolyPlatformEntity != physics.touches.abovePolyPlatformEntity;

    bool wasOnPlatform = physics.prevTouched.polyPlatform && !physics.touches.polyPlatform;

    if (!wasAbovePlatform && !wasOnPlatform)
        return;


    // entity MIGHT have fallen through polyPlatform
    auto platformEntity = wasAbovePlatform ? physics.prevTouched.abovePolyPlatformEntity : physics.prevTouched.polyPlatformEntity;

    PolyPlatform *platform  = room->entities.try_get<PolyPlatform>(platformEntity);
    Polyline *line          = room->entities.try_get<Polyline>(platformEntity);
    AABB *platformAABB      = room->entities.try_get<AABB>(platformEntity);

    if (!platform || !line || !platformAABB)
        return; // platform doesn't exist anymore

    bool
        fallTroughIntended          = platform->allowFallThrough && physics.ignorePolyPlatforms,
        entityWalkedOfThePlatform   = aabb.center.x <= platformAABB->bottomLeft().x || aabb.center.x > platformAABB->bottomRight().x;

    if (fallTroughIntended || entityWalkedOfThePlatform)
        return;


    // is the entity movement valid
    int platformHeight = platform->heightAtX(aabb.center.x, *line, *platformAABB);
    if (platformHeight == -1)
        return;

    int newY = platformHeight + aabb.halfSize.y + 1;
    bool invalidNewPosition = newY < aabb.center.y && (!wasOnPlatform || physics.velocity.y > 0);

    if (invalidNewPosition)
        return;

    physics.pixelsMovedByPolyPlatform.y += newY - aabb.center.y;

    aabb.center.y = newY;

    updateTerrainCollisions(physics, aabb);
}
