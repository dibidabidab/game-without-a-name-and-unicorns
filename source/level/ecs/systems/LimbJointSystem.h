
#ifndef GAME_LIMBJOINTSYSTEM_H
#define GAME_LIMBJOINTSYSTEM_H

#include "../../room/Room.h"
#include "EntitySystem.h"
#include "../components/LimbJoint.h"
#include "../components/Physics.h"
#include "../components/Leg.h"
#include "../components/Arm.h"

/**
 * see LimbJoint documentation
 */
class LimbJointSystem : public EntitySystem
{

    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override
    {
        room->entities.view<LimbJoint, AABB>().each([&](const LimbJoint &knee, AABB &aabb) {

            AABB
                *hip = room->entities.try_get<AABB>(knee.hipJointEntity),
                *foot = room->entities.try_get<AABB>(knee.footEntity);

            Leg *leg = room->entities.try_get<Leg>(knee.footEntity);
            Arm *arm = room->entities.try_get<Arm>(knee.footEntity);

            if (!hip || !foot || (!leg && !arm))
                return;

            float limbLength = leg ? leg->length : arm->length;

            vec2 pos0, pos1;

            float hipFootDistance = length(vec2(hip->center - foot->center));

            if (hipFootDistance == 0)
            {
                pos0 = pos1 = aabb.center; /// reuse previous position
            }
            else if (hipFootDistance < limbLength)
            {
                float radius = limbLength / 2;
                mu::circleIntersections(pos0, pos1, hip->center, foot->center, radius, radius, hipFootDistance);
            }
            else
            {
                pos0 = pos1 = vec2(foot->center) * .5f + vec2(hip->center) * .5f;
            }
            aabb.center = knee.invert ? pos0 : pos1;
        });
    }

};


#endif
