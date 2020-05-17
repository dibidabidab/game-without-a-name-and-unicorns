
#ifndef GAME_BOWWEAPONSYSTEM_H
#define GAME_BOWWEAPONSYSTEM_H

#include "EntitySystem.h"
#include "../../room/Room.h"
#include "../components/combat/Bow.h"
#include "../components/physics/Physics.h"
#include "../components/graphics/AsepriteView.h"
#include "../components/combat/Aiming.h"

class BowWeaponSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override
    {
        room->entities.view<Bow, AABB, AsepriteView>().each([&](Bow &bow, AABB &aabb, AsepriteView &sprite) {

            AABB *archerAABB = room->entities.try_get<AABB>(bow.archer);
            Aiming *aim = room->entities.try_get<Aiming>(bow.archer);
            if (!archerAABB)
                return; // bow is not being held by an valid entity.

            vec2 aimDirection = normalize(vec2(aim->target - archerAABB->center));

            aabb.center = archerAABB->center + ivec2(aimDirection * bow.distanceFromArcher);
        });
    }

};


#endif
