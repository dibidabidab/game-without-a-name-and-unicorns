
#ifndef GAME_ARROWSYSTEM_H
#define GAME_ARROWSYSTEM_H

#include "../EntitySystem.h"
#include "../../../level/room/Room.h"
#include "../../components/combat/Arrow.yaml"
#include "../../components/physics/Physics.h"
#include "../../components/graphics/AsepriteView.h"
#include "../../components/combat/Aiming.yaml"
#include "../../components/Spawning.yaml"
#include "../../components/Polyline.h"
#include "../../components/combat/Health.h"
#include "../../components/SoundSpeaker.yaml"

class ArrowSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *room) override
    {
        room->entities.view<Arrow, AABB, Physics, AsepriteView>().each([&](
            auto e, Arrow &arrow, AABB &aabb, Physics &physics, AsepriteView &sprite
        ){
            if (physics.touches.anything) // Terrain
            {
                room->entities.remove<Physics>(e);
                room->entities.assign<DespawnAfter>(e, mu::random(60, 100));
                auto &s = room->entities.assign<SoundSpeaker>(e, asset<au::Sound>("sounds/arrow_hit"));
                s.pitch = mu::random(.8, 1.2);
                s.volume = .18;
                return;
            }
            if (arrow.prevPos == ivec2(0)) // assume arrow.prevPos was not set before
                arrow.prevPos = aabb.center;

            bool enemyHit = false;
            room->entities.view<AABB, Health>().each([&](
                auto eOther, AABB &aabbOther, Health &healthOther
            ){
                if (enemyHit || (eOther == arrow.launchedBy) != arrow.returnToSender || !aabbOther.lineIntersectsOrInside(aabb.center, arrow.prevPos))
                    return;

                if (!healthOther.doesTakeDamageType(arrow.damageType))
                {
                    // enemy does not take this type of damage -> reflect arrow!
                    physics.velocity = normalize(physics.velocity) * arrow.launchVelocity * -.5f;
                    arrow.returnToSender = !arrow.returnToSender;
                    return;
                }

                healthOther.attacks.push_back({
                    arrow.damageType,
                    1,
                    arrow.launchedBy,
                    vec2(aabb.center) - physics.velocity
                });
                enemyHit = true;
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
            arrow.prevPos = aabb.center;
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
