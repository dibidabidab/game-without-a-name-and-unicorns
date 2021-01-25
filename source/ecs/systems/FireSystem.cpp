
#include <ecs/EntityEngine.h>
#include "FireSystem.h"
#include "../../generated/Fire.hpp"
#include "../../generated/Physics.hpp"
#include <generated/Spawning.hpp>
#include <generated/SoundSpeaker.hpp>

void FireSystem::update(double deltaTime, EntityEngine *engine)
{
    updateFrequency = 60;
    engine->entities.view<Fire, AABB>().each([&] (auto e, Fire &fire, AABB &aabb) {

        for (int i = 0; i < fire.intensity * 2 * (1 + fire.width * .8) * mu::random(mu::random(mu::random())); i++)
        {
            auto particle = engine->entities.create();

            bool sound = mu::random() > .98;
            if (sound)
            {
                SoundSpeaker &sound = engine->entities.assign_or_replace<SoundSpeaker>(particle);
                sound.sound.set("sounds/fire/fire0");
//                sound.volume = .6; set in update loop below
                sound.pitch = mu::random(.5, 1.5);
            }

            engine->entities.assign<FireParticle>(particle);
            AABB &particleAABB = engine->entities.assign<AABB>(particle);
            particleAABB.center = aabb.center;
            particleAABB.center.x += mu::randomInt(-fire.width * .5, fire.width * .5);
            particleAABB.halfSize = ivec2(1);
            Physics &particlePhysics = engine->entities.assign<Physics>(particle);
            particlePhysics.ignoreTileTerrain = true;
            particlePhysics.gravity = mu::random(-280, -50);
            particlePhysics.moveByWind = 500;
            particlePhysics.velocity = vec2(mu::random(-40, 40) * mu::random(.2, 1.2), mu::random(-50, 100));
            engine->entities.assign<DespawnAfter>(particle).time = log(fire.intensity + 1) * mu::random(.5, 1.4) * (sound ? 3. : 1.);
        }
    });

    engine->entities.view<FireParticle, SoundSpeaker, DespawnAfter>().each([&](auto, SoundSpeaker &sound, DespawnAfter &despawn) {
        float age = despawn.timer / despawn.time;
        sound.volume = min(1. - age, age * 2.) * .1;
    });
}
