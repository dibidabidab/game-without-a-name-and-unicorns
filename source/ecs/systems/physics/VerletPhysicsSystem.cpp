
#include "VerletPhysicsSystem.h"
#include <glm/gtx/fast_square_root.hpp>

void VerletPhysicsSystem::init(EntityEngine *engine)
{
    room = (TiledRoom *) engine;
    updateFrequency = 60;

    room->entities.on_construct<VerletRope>().connect<&VerletPhysicsSystem::onPotentionalChildRopeCreation>(this);
    room->entities.on_construct<AttachToRope>().connect<&VerletPhysicsSystem::onPotentionalChildRopeCreation>(this);

    afterLoad = room->afterLoad += [&] {
        for (int i = 0; i < updateFrequency * 5; i++)
            update(1.f / updateFrequency, room);
    };
}

void VerletPhysicsSystem::update(double deltaTime, EntityEngine *)
{
    room->entities.view<AABB, VerletRope>().each([&] (AABB &aabb, VerletRope &rope) {

        if (rope.isAttachedToRope || rope.length == 0)
            return;

        updateRope(rope, aabb, deltaTime);
    });


    room->entities.view<AttachToRope, AABB>().each([&](auto e, AttachToRope &attach, AABB &aabb) {

        if (attach.ropeEntity == entt::null)
            return;

        auto *thisRope = room->entities.try_get<VerletRope>(e);

        if (attach.registerAsChildRope)
        {
            auto *parentRope = room->entities.try_get<VerletRope>(attach.ropeEntity);

            if (parentRope && thisRope)
            {
                parentRope->attachedRopes.push_back(e);
                attach.registerAsChildRope = false;
                thisRope->isAttachedToRope = true;
            }
        }

        VerletRope *rope = room->entities.try_get<VerletRope>(attach.ropeEntity);
        if (!rope || (thisRope && thisRope->isAttachedToRope))
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
    if (rope.length == 0)
        return;

    attach.x = min<float>(1, max<float>(0, attach.x));

    float pointIndex = (rope.nrOfPoints - 1) * attach.x;

    int
            pointIndex0 = pointIndex,
            pointIndex1 = ceil(pointIndex);

    if (rope.points.size() <= max(pointIndex0, pointIndex1))
        return;

    auto &point0Pos = rope.points[pointIndex0].currentPos;
    auto &point1Pos = rope.points[pointIndex1].currentPos;

    aabb.center = point0Pos;
    aabb.center += (point1Pos - point0Pos) * (pointIndex - pointIndex0);
    aabb.center += attach.offset;
}

void VerletPhysicsSystem::updateRope(VerletRope &rope, AABB &aabb, float deltaTime)
{
    if (rope.nrOfPoints < 2) rope.nrOfPoints = 2;

    float segmentLength = rope.length / (rope.nrOfPoints - 1);

    AABB *endPointAABB = NULL;
    if (rope.endPointEntity != entt::null)
        endPointAABB = room->entities.try_get<AABB>(rope.endPointEntity);

    if (rope.points.size() != rope.nrOfPoints)
    {
        bool addedPoints = false;

        while (rope.points.size() > rope.nrOfPoints)
            rope.points.pop_back();

        while (rope.points.size() < rope.nrOfPoints)
        {
            addedPoints = true;
            rope.points.emplace_back();
            auto &p = rope.points.back();
            int size = rope.points.size();
            if (size == 1)
                p.currentPos = p.prevPos = aabb.center;
            else if (!rope.fixedEndPoint || !endPointAABB)
                p.currentPos = p.prevPos = rope.points[size - 2].currentPos + normalize(rope.gravity) * segmentLength;
            else
            {
                vec2 diff = endPointAABB->center - aabb.center;
                p.currentPos = p.prevPos = vec2(aabb.center) + (diff * vec2(size / rope.nrOfPoints));
            }
        }

        if (endPointAABB && addedPoints && !rope.fixedEndPoint)
            endPointAABB->center = rope.points.back().currentPos;
    }

    if (endPointAABB)
    {
        auto &endPoint = rope.points.back();
        if (ivec2(endPoint.currentPos) != endPointAABB->center)
            endPoint.currentPos = endPointAABB->center;
    }

    for (int i = 1; i < rope.nrOfPoints; i++)
    {
        auto &p = rope.points[i];

        vec2 velocity = (p.currentPos - p.prevPos) * rope.friction;
        if (rope.moveByWind != 0)
            velocity += room->getWindMap().getAtPixelCoordinates(p.currentPos.x, p.currentPos.y) * deltaTime * rope.moveByWind;

        p.prevPos = p.currentPos;
        p.currentPos += velocity;
        p.currentPos += rope.gravity * deltaTime;
    }

    rope.points[0].currentPos = aabb.center; // todo: add possibility for offset

    for (int j = 0; j < rope.updatePrecision; j++)
    {
        for (int i = 0; i < rope.nrOfPoints - 1; i++)
        {
            auto &p0 = rope.points[i], &p1 = rope.points[i + 1];

            float dist = fastLength(p0.currentPos - p1.currentPos);
            float error = abs(dist - segmentLength);

            vec2 changeDir;
            if (dist > segmentLength)
                changeDir = fastNormalize(p0.currentPos - p1.currentPos);
            else
                changeDir = fastNormalize(p1.currentPos - p0.currentPos);

            vec2 change = changeDir * error;

            if (i == 0)
                p1.currentPos += change;
            else
            {
                p0.currentPos -= change * .5f;
                p1.currentPos += change * .5f;
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
                if (childRope)
                    childRope->isAttachedToRope = false;
                continue;
            }

            updateAttachToRope(rope, *childAttach, *childAABB);
            updateRope(*childRope, *childAABB, deltaTime);
        }
    }

    if (endPointAABB && !rope.fixedEndPoint)
        endPointAABB->center = rope.points.back().currentPos;
}
