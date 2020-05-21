
#ifndef GAME_ARROWSYSTEM_H
#define GAME_ARROWSYSTEM_H

#include "../EntitySystem.h"
#include "../../../room/Room.h"
#include "../../components/combat/Arrow.h"
#include "../../components/physics/Physics.h"
#include "../../components/graphics/AsepriteView.h"
#include "../../components/combat/Aiming.h"
#include "../../components/DespawnAfter.h"
#include "../../components/Polyline.h"
#include "../../components/combat/Health.h"
#include "../../components/combat/KnockBack.h"

class ArrowSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, Room *room) override
    {
        room->entities.view<Arrow, AABB, Physics, AsepriteView>().each([&](
            auto e, Arrow &arrow, AABB &aabb, Physics &physics, AsepriteView &sprite
        ){
            if (physics.touches.anything) // Terrain
            {
                room->entities.remove<Physics>(e);
                room->entities.assign<DespawnAfter>(e, mu::random(60, 100));
                return;
            }

            bool enemyHit = false;
            room->entities.view<AABB, Health>().each([&](
                auto eOther, AABB &aabbOther, Health &healthOther
            ){
                if (!aabb.overlaps(aabbOther))
                    return;

                enemyHit = true;
                healthOther.curHealth -= 5;

                KnockBack *knockBack = room->entities.try_get<KnockBack>(eOther);
                if (knockBack)
                    knockBack->add(100, normalize(physics.velocity));

//                if (healthOther.curHealth <= 0.0f)  // todo, this should not be responsibility of ArrowSystem
//                    room->entities.destroy(eOther);
            });
            if (enemyHit)
            {
                room->entities.destroy(e);
                return;
            }
            // choose sprite frame based on velocity:
            vec2 dir = normalize(physics.velocity);
            float angle = atan2(dir.y, dir.x) + mu::PI;

            int frame = round(angle / (2 * mu::PI) * sprite.sprite->frameCount);
            frame = max(0, frame);
            if (frame == sprite.sprite->frameCount)
                frame = 0;
            sprite.frame = frame;
        });

        addTrailPointsTimer += deltaTime;
        bool addTrailPoint = addTrailPointsTimer > .02;
        if (addTrailPoint)
            addTrailPointsTimer = 0;

        room->entities.view<Arrow, AABB, Polyline, Physics>().each([&](
            auto &, const AABB &aabb, Polyline &line, Physics &physics
        ) {
            if (addTrailPoint || line.points.empty())
                line.points.emplace_back();
            line.points.back() = vec2(aabb.center) - normalize(physics.velocity) * 12.f;
            if (line.points.size() > 3)
                line.points.pop_front();
        });

        if (addTrailPoint) room->entities.view<Arrow, Polyline>(entt::exclude<Physics>).each([&](
            auto &, Polyline &line
        ) {
            if (!line.points.empty())
                line.points.pop_front();
        });
    }

    float addTrailPointsTimer = 0;

};


#endif
