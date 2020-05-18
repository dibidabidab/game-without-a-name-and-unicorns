
#ifndef GAME_ARMSSYSTEM_H
#define GAME_ARMSSYSTEM_H

#include "../EntitySystem.h"
#include "../../../room/Room.h"
#include "../../components/body_parts/Arm.h"
#include "../../components/physics/Physics.h"

class ArmsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override
    {

        room->entities.view<Arm, AABB>().each([&](Arm &arm, AABB &handAABB) {

            AABB *grabTarget = room->entities.try_get<AABB>(arm.grab);
            AABB *bodyAABB = room->entities.try_get<AABB>(arm.body);

            if (!bodyAABB)
                return; // arm is not attached to body

            if (grabTarget)
            {
                // the hand is grabbig something -> set position of hand to the grabben entity:
                handAABB.center = grabTarget->center;
            }
            else
            {
                // todo: default arm/hand movement

                handAABB.center = bodyAABB->center + arm.anchor;
                handAABB.center.x += (arm.anchor.x > 0 ? 1 : -1) * arm.length * .6;
            }

        });
    }

};


#endif
