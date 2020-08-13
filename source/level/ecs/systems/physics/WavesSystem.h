
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

    Room *room;

  protected:

    void update(double deltaTime, Room *room) override
    {
        this->room = room;
        room->entities.view<Polyline, PolylineWaves, AABB>().each([&](
                Polyline &line, PolylineWaves &wave, AABB &aabb
        ){
            float fakeDeltaTime = deltaTime;
            constexpr float maxDeltaTime = 1. / 120.;

            while (fakeDeltaTime > maxDeltaTime)
            {
                updateWaves(maxDeltaTime, line, wave, aabb);
                fakeDeltaTime -= maxDeltaTime;
            }
            updateWaves(fakeDeltaTime, line, wave, aabb);
        });

        room->entities.view<PolyPlatform, Polyline, PolylineWaves, AABB>().each([&](
                PolyPlatform &platform, Polyline &line, PolylineWaves &wave, AABB &aabb
        ){
            for (auto e : platform.entitiesOnPlatform)
            {
                const AABB *onPlatformAABB = room->entities.try_get<AABB>(e);
                const Physics *onPlatformPhysics = room->entities.try_get<Physics>(e);

                if (!onPlatformAABB || !onPlatformPhysics)
                    continue;

                if (!onPlatformPhysics->prevTouched.polyPlatform && onPlatformPhysics->prevVelocity.y < 0)
                {
                    int xIndex = onPlatformAABB->center.x - aabb.center.x;

                    int i = 0;
                    bool left = false;
                    for (vec2 &p : line.points)
                    {
                        if (p.x < xIndex)
                            left = true;
                        if (left && p.x > xIndex)
                        {
                            wave.springs[i].velocity += onPlatformPhysics->prevVelocity.y * wave.impactMultiplier;
                            break;
                        }
                        i++;
                    }
                }
            }
        });
    }

    void updateWaves(float deltaTime, Polyline &line, PolylineWaves &wave, AABB &aabb)
    {
        int i = 0;
        for (vec2 &p : line.points)
        {
            if (i == wave.springs.size())
                wave.springs.emplace_back();

            PolylineWaves::Spring &spring = wave.springs.at(i);

            // x = height - targetHeight;
            float x = p.y - (p.y - spring.yOffset);

            float acceleration = -wave.stiffness * x;

            spring.velocity *= max(0., 1. - wave.dampening * deltaTime);

            p.y += spring.velocity * deltaTime;
            spring.yOffset += spring.velocity * deltaTime;
            spring.velocity += acceleration * deltaTime;

            if (wave.moveByWind != 0)
            {
                spring.velocity += wave.moveByWind * room->getMap().wind.getAtPixelCoordinates(p.x + aabb.center.x, p.y + aabb.center.y).y;
            }
            i++;
        }
        wave.springs.resize(i);

        // propagate waves:

        std::vector<float> leftDeltas(wave.springs.size());
        std::vector<float> rightDeltas(wave.springs.size());

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
    }

};


#endif
