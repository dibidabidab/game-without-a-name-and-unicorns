
#include <ecs/EntityEngine.h>
#include "FireSystem.h"
#include "../../generated/Fire.hpp"
#include "../../generated/Physics.hpp"
#include <generated/Spawning.hpp>

void FireSystem::update(double deltaTime, EntityEngine *engine)
{
    updateFrequency = 20;
    engine->entities.view<Fire, AABB>().each([&] (auto e, Fire &fire, AABB &aabb) {

        for (int i = 0; i < fire.intensity * 2 * fire.width * mu::random(); i++)
        {
            auto particle = engine->createChild(e);
            engine->entities.assign<FireParticle>(particle);
            AABB &particleAABB = engine->entities.assign<AABB>(particle);
            particleAABB.center = aabb.center;
            particleAABB.center.x += mu::randomInt(-fire.width * .5, fire.width * .5);
            particleAABB.halfSize = ivec2(1);
            Physics &particlePhysics = engine->entities.assign<Physics>(particle);
            particlePhysics.ignoreTileTerrain = true;
            particlePhysics.gravity = mu::random(-200, -50);
            particlePhysics.moveByWind = 400;
            particlePhysics.velocity = vec2(mu::random(-80, 80) * mu::random(.2, 1.2), mu::random(-50, 100));
            engine->entities.assign<DespawnAfter>(particle).time = log(fire.intensity + 1) * mu::random(.8, 1.8);
        }
    });
}
