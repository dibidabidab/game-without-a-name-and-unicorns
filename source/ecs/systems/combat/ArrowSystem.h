
#ifndef GAME_ARROWSYSTEM_H
#define GAME_ARROWSYSTEM_H

#include <ecs/systems/EntitySystem.h>
#include "../../../tiled_room/TiledRoom.h"
#include "../../../generated/Health.hpp"
#include <generated/Spawning.hpp>
#include <generated/SoundSpeaker.hpp>
#include "../../../generated/Arrow.hpp"
#include "../../components/component_methods.h"

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
                room->entities.assign<DespawnAfter>(e).time = mu::random(60, 100);
                auto &s = room->entities.assign<SoundSpeaker>(e);
                s.sound = asset<au::Sound>("sounds/arrow_hit");
                s.pitch = mu::random(.8, 1.2);
                s.volume = .18;
                room->emitEntityEvent(e, 0, "TerrainHit");
                return;
            }
            if (arrow.prevPos == ivec2(0)) // assume arrow.prevPos was not set before
                arrow.prevPos = aabb.center;

            bool enemyHit = false;
            room->entities.view<AABB, Health>().each([&](
                auto eOther, AABB &aabbOther, Health &healthOther
            ){
                if (enemyHit || (eOther == arrow.launchedBy) != arrow.returnToSender || !lineIntersectsOrInside(aabbOther, aabb.center, arrow.prevPos))
                    return;

                if (!healthOther.doesTakeDamageType(arrow.damageType))
                {
                    // enemy does not take this type of damage -> reflect arrow!
                    physics.velocity = normalize(physics.velocity) * arrow.launchVelocity * -.5f;
                    arrow.returnToSender = !arrow.returnToSender;
                    {
                        auto se = room->entities.create();
                        room->entities.assign<DespawnAfter>(se).time = 1.5;
                        auto &s = room->entities.assign<SoundSpeaker>(se);
                        s.sound = asset<au::Sound>("sounds/arrow_block");
                        s.pitch = mu::random(.7, 1.1);
                    }
                    room->emitEntityEvent(e, 0, "Reflected");
                    return;
                }

                Damage attack;
                attack.points = 1;
                attack.type = arrow.damageType;
                attack.dealtBy = arrow.launchedBy;
                attack.sourcePosition = vec2(aabb.center) - physics.velocity;
                healthOther.attacks.push_back(attack);
                enemyHit = true;
                room->emitEntityEvent(e, eOther, "EnemyHit");
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
