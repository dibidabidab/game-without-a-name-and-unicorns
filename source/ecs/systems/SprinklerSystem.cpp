
#include <ecs/EntityEngine.h>
#include "SprinklerSystem.h"
#include "../../generated/Sprinkler.hpp"
#include "../../generated/Physics.hpp"
#include "../../generated/BloodDrop.hpp"
#include <generated/Spawning.hpp>

void SprinklerSystem::update(double deltaTime, EntityEngine *engine)
{
    updateFrequency = 30;

    engine->entities.view<AABB, Sprinkler>().each([&](AABB &spklrAABB, Sprinkler &spklr) {

        int nrOfDrops = spklr.amount * mu::random(1.1);
        for (int i = 0; i < nrOfDrops; i++)
        {
            auto dropE = engine->entities.create();
            auto &drop = engine->entities.assign<BloodDrop>(dropE);
            drop.size = mu::random(spklr.minDropSize, spklr.maxDropSize);
            drop.permanentDrawOnTerrain = spklr.permanentDrawOnTerrain;
            drop.split = false;
            drop.color = spklr.dropColor;

            engine->entities.assign<AABB>(dropE).center = spklrAABB.center
                    + ivec2(mu::randomInt(-spklr.maxRandomOffset.x, spklr.maxRandomOffset.x),
                            mu::randomInt(-spklr.maxRandomOffset.y, spklr.maxRandomOffset.y));

            auto &physics = engine->entities.assign<Physics>(dropE);
            physics.velocity = rotate(vec2(0, spklr.force * mu::random(.8, 1.2)), mu::random(spklr.angle - spklr.angleRange * .5, spklr.angle + spklr.angleRange * .5) * mu::DEGREES_TO_RAD);
            physics.gravity = spklr.gravity;
            physics.ignoreTileTerrain = true;

            engine->entities.assign<DespawnAfter>(dropE).time = mu::random(spklr.minLifetime, spklr.maxLifetime);
        }
    });

}
