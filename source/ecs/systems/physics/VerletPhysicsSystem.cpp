
#include "VerletPhysicsSystem.h"

void VerletPhysicsSystem::init(EntityEngine *engine)
{
    room = (Room *) engine;
    updateFrequency = 60;


    room->entities.on_construct<VerletRope>().connect<&VerletPhysicsSystem::onPotentionalChildRopeCreation>(this);
    room->entities.on_construct<AttachToRope>().connect<&VerletPhysicsSystem::onPotentionalChildRopeCreation>(this);
}

void VerletPhysicsSystem::update(double deltaTime, EntityEngine *)
{
    room->entities.view<AABB, VerletRope>().each([&] (AABB &aabb, VerletRope &rope) {

        if (rope.isAttachedToRope)
            return;

        updateRope(rope, aabb, deltaTime);
    });


    room->entities.view<AttachToRope, AABB>().each([&](auto e, AttachToRope &attach, AABB &aabb) {

        if (attach.ropeEntity == entt::null)
            return;

        if (attach.registerAsChildRope)
        {
            auto *parentRope = room->entities.try_get<VerletRope>(attach.ropeEntity);
            auto *thisRope = room->entities.try_get<VerletRope>(e);

            if (parentRope && thisRope)
            {
                parentRope->attachedRopes.push_back(e);
                attach.registerAsChildRope = false;
                thisRope->isAttachedToRope = true;
            }
        }

        VerletRope *rope = room->entities.try_get<VerletRope>(attach.ropeEntity);
        if (!rope || rope->isAttachedToRope)
            return;

        updateAttachToRope(*rope, attach, aabb);
    });
}

void VerletPhysicsSystem::onPotentionalChildRopeCreation(entt::registry &reg, entt::entity e)
{
    auto *attachToRope = reg.try_get<AttachToRope>(e);
    if (!attachToRope)
        return;
    auto *rope = reg.try_get<VerletRope>(e);
    if (!rope)
        return;

    // THIS IS A ROPE THAT IS ATTACHED TO ANOTHER ROPE. (a tree branch)

    attachToRope->registerAsChildRope = true;
}

void VerletPhysicsSystem::updateAttachToRope(const VerletRope &rope, AttachToRope &attach, AABB &aabb)
{
    attach.x = min<float>(1, max<float>(0, attach.x));

    float pointIndex = (rope.nrOfPoints - 1) * attach.x;

    int
            pointIndex0 = pointIndex,
            pointIndex1 = ceil(pointIndex);

    auto &point0Pos = rope.points[pointIndex0].currentPos;
    auto &point1Pos = rope.points[pointIndex1].currentPos;

    aabb.center = point0Pos;
    aabb.center += (point1Pos - point0Pos) * (pointIndex - pointIndex0);
    aabb.center += attach.offset;
}

void VerletPhysicsSystem::updateRope(VerletRope &rope, AABB &aabb, double deltaTime)
{
    if (rope.nrOfPoints < 2) rope.nrOfPoints = 2;

    float segmentLength = rope.length / (rope.nrOfPoints - 1);

    while (rope.points.size() > rope.nrOfPoints)
        rope.points.pop_back();

    bool addedPoints = false;
    while (rope.points.size() < rope.nrOfPoints)
    {
        addedPoints = true;
        rope.points.emplace_back();
        auto &p = rope.points.back();
        int size = rope.points.size();
        if (size == 1)
            p.currentPos = p.prevPos = aabb.center;
        else
            p.currentPos = p.prevPos = rope.points[size - 2].currentPos + normalize(rope.gravity) * segmentLength;
    }

    AABB *endPointAABB = NULL;
    if (rope.endPointEntity != entt::null)
        endPointAABB = room->entities.try_get<AABB>(rope.endPointEntity);
    if (endPointAABB && addedPoints)
        endPointAABB->center = rope.points.back().currentPos;

    for (int i = 0; i < rope.nrOfPoints; i++)
    {
        auto &p = rope.points[i];

        if (i == rope.nrOfPoints - 1 && endPointAABB)
        {
            if (ivec2(p.currentPos) != endPointAABB->center)
                p.currentPos = endPointAABB->center;
        }

        vec2 velocity = (p.currentPos - p.prevPos) * vec2(rope.friction);
        if (rope.moveByWind != 0)
            velocity += room->getMap().wind.getAtPixelCoordinates(p.currentPos.x, p.currentPos.y) * vec2(deltaTime * rope.moveByWind);

        p.prevPos = p.currentPos;
        p.currentPos += velocity;
        p.currentPos += rope.gravity * vec2(deltaTime);
    }

    rope.points[0].currentPos = aabb.center; // todo: add possibility for offset

    for (int j = 0; j < 1; j++) // todo
    {
        for (int i = 0; i < rope.nrOfPoints - 1; i++)
        {
            auto &p0 = rope.points[i], &p1 = rope.points[i + 1];

            float dist = length(p0.currentPos - p1.currentPos);
            float error = abs(dist - segmentLength);

            vec2 changeDir;
            if (dist > segmentLength)
                changeDir = normalize(p0.currentPos - p1.currentPos);
            else
                changeDir = normalize(p1.currentPos - p0.currentPos);

            vec2 change = changeDir * vec2(error);

            if (i == 0)
                p1.currentPos += change;
            else
            {
                p0.currentPos -= change * vec2(.5);
                p1.currentPos += change * vec2(.5);
            }
        }

        // update attached ropes:
        for (int i = rope.attachedRopes.size() - 1; i >= 0; i--)
        {
            auto childRopeEntity = rope.attachedRopes[i];
            auto *childRope = room->entities.try_get<VerletRope>(childRopeEntity);
            auto *childAttach = room->entities.try_get<AttachToRope>(childRopeEntity);
            auto *childAABB = room->entities.try_get<AABB>(childRopeEntity);

            if (!childRope || !childAttach || !childAABB)
            {
                rope.attachedRopes[i] = rope.attachedRopes.back();
                rope.attachedRopes.pop_back();
                continue;
            }

            updateAttachToRope(rope, *childAttach, *childAABB);
            updateRope(*childRope, *childAABB, deltaTime);
        }
    }

    if (endPointAABB && !rope.fixedEndPoint)
        endPointAABB->center = rope.points.back().currentPos;
}
