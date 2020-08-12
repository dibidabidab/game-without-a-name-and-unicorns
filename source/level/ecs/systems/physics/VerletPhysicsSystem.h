
#ifndef GAME_VERLETPHYSICSSYSTEM_H
#define GAME_VERLETPHYSICSSYSTEM_H

#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/physics/Physics.h"
#include "../../components/physics/VerletRope.h"

/**
 * based on https://github.com/dci05049/Verlet-Rope-Unity/blob/master/Tutorial%20Verlet%20Rope/Assets/Rope.cs
 */
class VerletPhysicsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    void init(Room *room) override
    {
        updateFrequency = 60;
    }

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<AABB, VerletRope>().each([&] (AABB &aabb, VerletRope &rope) {

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

            rope.points[0].currentPos = aabb.center;

            for (int j = 0; j < 20; j++)
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
            }

            if (endPointAABB && !rope.fixedEndPoint)
                endPointAABB->center = rope.points.back().currentPos;
        });
        room->entities.view<AttachToRope, AABB>().each([&](AttachToRope &attach, AABB &aabb) {

            if (attach.ropeEntity == entt::null)
                return;

            VerletRope *rope = room->entities.try_get<VerletRope>(attach.ropeEntity);
            if (!rope)
                return;

            attach.x = min<float>(1, max<float>(0, attach.x));

            float pointIndex = (rope->nrOfPoints - 1) * attach.x;

            int
                pointIndex0 = pointIndex,
                pointIndex1 = ceil(pointIndex);

            auto &point0Pos = rope->points.at(pointIndex0).currentPos;
            auto &point1Pos = rope->points.at(pointIndex1).currentPos;

            aabb.center = point0Pos;
            aabb.center += (point1Pos - point0Pos) * (pointIndex - pointIndex0);
            aabb.center += attach.offset;
        });
    }
};


#endif
