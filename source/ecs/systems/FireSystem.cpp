
#include <ecs/EntityEngine.h>
#include "FireSystem.h"
#include "../../generated/Fire.hpp"
#include "../../generated/Physics.hpp"
#include "../../generated/Audio.hpp"
#include <generated/Spawning.hpp>
#include <generated/SoundSpeaker.hpp>

void FireSystem::update(double deltaTime, EntityEngine *engine)
{
    updateFrequency = 60;
    {
        gu::profiler::Zone z("particles");

        engine->entities.view<Fire, AABB>().each([&] (auto e, Fire &fire, AABB &aabb) {

            for (int i = 0; i < fire.intensity * 2 * (1 + fire.width * .8) * mu::random(mu::random(mu::random())); i++)
            {
                auto particle = engine->entities.create();

                bool sound = mu::random() > .98;
                if (sound)
                {
                    SoundSpeaker &sound = engine->entities.assign_or_replace<SoundSpeaker>(particle);
                    engine->entities.assign<PositionedAudio>(particle);
                    sound.sound.set("sounds/fire/fire0");
    //                sound.volume = .6; set in update loop below
                    sound.pitch = mu::random(.5, 1.5);
                    sound.pauseOnLeavingRoom = true;
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

        engine->entities.view<Fire, Physics>().each([&] (auto e, auto, Physics &physics) {
            if (physics.touches.fluid)
                engine->entities.remove<Fire>(e);
        });

        engine->entities.view<FireParticle, SoundSpeaker, DespawnAfter>().each([&](auto, SoundSpeaker &sound, DespawnAfter &despawn) {
            float age = despawn.timer / despawn.time;
            sound.volume = min(1. - age, age * 2.) * .065;
        });
    }

    gu::profiler::Zone z("ignition");

    engine->entities.view<AABB, Flammable>(entt::exclude<Fire>).each([&](auto e, AABB &aabb, Flammable &flammable) {

        flammable.timer += deltaTime;

        if (flammable.timer >= flammable.checkInterval)
            flammable.timer = 0;
        else return;

        float rangeSq = flammable.range * flammable.range;

        bool ignited = false;

        engine->entities.view<AABB, Igniter>().each([&](auto igniterEntity, AABB &igniterAABB, Igniter &igniter) {

            if (ignited || igniter.igniteChance < mu::random())
                return;

            vec2 diff = igniterAABB.center - aabb.center;

            float ignRangeSq = igniter.range * igniter.range;

            if (length2(diff) <= rangeSq + ignRangeSq)
            {
                // IGNITE

                if (flammable.addedFireComponent.width > 0 && flammable.addedFireComponent.intensity > 0)
                    engine->entities.assign<Fire>(e, flammable.addedFireComponent).ignitedBy = igniterEntity;
                if (flammable.addedIgniterComponent.igniteChance > 0 && flammable.addedIgniterComponent.range > 0)
                    engine->entities.assign<Igniter>(e, flammable.addedIgniterComponent);

                engine->emitEntityEvent(e, igniterEntity, "Ignited");
                ignited = true;
            }
        });
    });
}
