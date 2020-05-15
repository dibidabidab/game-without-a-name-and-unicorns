
#ifndef GAME_KNEEJOINTSYSTEM_H
#define GAME_KNEEJOINTSYSTEM_H

#include "../../room/Room.h"
#include "EntitySystem.h"
#include "../components/KneeJoint.h"
#include "../components/Physics.h"
#include "../components/Leg.h"

class KneeJointSystem : public EntitySystem
{

    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override
    {
        room->entities.view<KneeJoint, AABB>().each([&](const KneeJoint &knee, AABB &aabb) {

            AABB
                *hip = room->entities.try_get<AABB>(knee.hipJointEntity),
                *foot = room->entities.try_get<AABB>(knee.footEntity);

            Leg *leg = room->entities.try_get<Leg>(knee.footEntity);

            if (!hip || !foot || !leg)
                return;

            vec2 pos0, pos1;

            float hipFootDistance = length(vec2(hip->center - foot->center));

            if (hipFootDistance < leg->length)
            {
                float radius = float(leg->length) / 2;
                mu::circleIntersections(pos0, pos1, hip->center, foot->center, radius, radius, hipFootDistance);
            }
            else
            {
                pos0 = pos1 = vec2(foot->center) * .5f + vec2(hip->center) * .5f;
            }
            aabb.center = pos1;
        });
    }

};


#endif
