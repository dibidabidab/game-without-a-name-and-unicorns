
#ifndef GAME_LIMBJOINTSYSTEM_H
#define GAME_LIMBJOINTSYSTEM_H

#include "../../../level/room/Room.h"
#include "../EntitySystem.h"
#include "../../components/body_parts/LimbJoint.yaml"
#include "../../components/physics/Physics.h"
#include "../../components/body_parts/Leg.h"
#include "../../components/body_parts/Arm.yaml"

/**
 * see LimbJoint documentation
 */
class LimbJointSystem : public EntitySystem
{

    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *room) override
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

            bool flipLimb = false;

            if (hipFootDistance == 0)
            {
                pos0 = pos1 = aabb.center; /// reuse previous position
            }
            else if (hipFootDistance < limbLength)
            {
                float radius = limbLength / 2;
                mu::circleIntersections(pos0, pos1, hip->center, foot->center, radius, radius, hipFootDistance);

                Flip *flip = room->entities.try_get<Flip>(leg ? leg->body : arm->body);
                if (flip && flip->horizontal)
                    flipLimb = true;
            }
            else
            {
                pos0 = pos1 = vec2(foot->center) * .5f + vec2(hip->center) * .5f;
            }
            bool usePos0 = leg ? knee.invert : !knee.invert;
            if (flipLimb)
                usePos0 = !usePos0;
            aabb.center = usePos0 ? pos0 : pos1;
        });
    }

};


#endif
