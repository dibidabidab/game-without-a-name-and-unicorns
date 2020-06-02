
#ifndef GAME_WAVESSYSTEM_H
#define GAME_WAVESSYSTEM_H

#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/physics/PolyPlatform.h"
#include "../../components/physics/Physics.h"

/**
 * based on:
 * https://gamedevelopment.tutsplus.com/tutorials/make-a-splash-with-dynamic-2d-water-effects--gamedev-236
 */
class WavesSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<PolyPlatform, Polyline, PolyPlatformWaves, AABB>().each([&](

            PolyPlatform &platform, Polyline &line, PolyPlatformWaves &wave, AABB &aabb

        ){
            int i = 0;
            for (vec2 &p : line.points)
            {
                if (i == wave.springs.size())
                    wave.springs.emplace_back();

                PolyPlatformWaves::Spring &spring = wave.springs.at(i);

                // x = height - targetHeight;
                float x = p.y - (p.y - spring.yOffset);

                float acceleration = -wave.stiffness * x;

                spring.velocity *= max(0., 1. - wave.dampening * deltaTime);

                p.y += spring.velocity * deltaTime;
                spring.yOffset += spring.velocity * deltaTime;
                spring.velocity += acceleration * deltaTime;

                i++;
            }
            wave.springs.resize(i);

            // propagate waves:

            float leftDeltas[wave.springs.size()];
            float rightDeltas[wave.springs.size()];

            for (int j = 0; j < 8; j++)
            {
                i = 0;
                for (vec2 &p : line.points)
                {
                    if (i > 0)
                    {
                        leftDeltas[i] =
                                wave.spread * deltaTime * (wave.springs[i].yOffset - wave.springs[i - 1].yOffset);
                        wave.springs[i - 1].velocity += leftDeltas[i];
                    }
                    if (i < wave.springs.size() - 1)
                    {
                        rightDeltas[i] =
                                wave.spread * deltaTime * (wave.springs[i].yOffset - wave.springs[i + 1].yOffset);
                        wave.springs[i + 1].velocity += rightDeltas[i];
                    }
                    i++;
                }
            }

            for (auto e : platform.entitiesOnPlatform)
            {
                const AABB *onPlatformAABB = room->entities.try_get<AABB>(e);
                const Physics *onPlatformPhysics = room->entities.try_get<Physics>(e);

                if (!onPlatformAABB || !onPlatformPhysics)
                    continue;

                if (!onPlatformPhysics->prevTouched.polyPlatform && onPlatformPhysics->prevVelocity.y < 0)
                {
                    int xIndex = onPlatformAABB->center.x - aabb.center.x;

                    i = 0;
                    bool left = false;
                    for (vec2 &p : line.points)
                    {
                        if (p.x < xIndex)
                            left = true;
                        if (left && p.x > xIndex)
                        {
                            wave.springs[i].velocity += onPlatformPhysics->prevVelocity.y;
                            break;
                        }
                        i++;
                    }
                }
            }
        });
    }

};


#endif
