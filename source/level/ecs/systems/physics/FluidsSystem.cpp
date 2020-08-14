
#include "FluidsSystem.h"
#include "../../components/SoundSpeaker.h"
#include "../../components/Spawning.h"
#include "../../components/graphics/BloodDrop.h"
#include "../../components/graphics/AsepriteView.h"

void FluidsSystem::update(double deltaTime, Room *room)
{
    this->room = room;

    room->entities.view<FluidBubbleParticle, Physics, AsepriteView>()
            .each([&](auto e, FluidBubbleParticle &particle, Physics &physics, AsepriteView &sprite) {

                if (particle.timeBeforeDestroy != 0)
                {
                    particle.timeBeforeDestroy -= deltaTime;
                    if (particle.timeBeforeDestroy <= 0)
                        room->entities.destroy(e);
                }
                else
                {
                    particle.timeAlive -= deltaTime;
                    if (particle.timeAlive <= 0 || !physics.touches.fluid)
                    {
                        sprite.loop = false;
                        particle.timeBeforeDestroy = sprite.playTag("pop");
                    }
                }
            });

    room->entities.view<Fluid, AABB>().each([&](Fluid &fluid, AABB &aabb) {

        auto updateEntitiesInFluid = [&](auto &entities) {
            for (auto e : entities)
            {
                Physics *physics = room->entities.try_get<Physics>(e);
                AABB *bodyInFluid = room->entities.try_get<AABB>(e);

                if (!physics || !bodyInFluid || !physics->fluidAnimations)
                    continue;

                if (physics->touches.fluid && !physics->prevTouched.fluid && physics->velocity.y < 0)
                    spawnFluidSplash(fluid.enterSound, *physics, *bodyInFluid, fluid);
                else if (physics->prevTouched.fluid && !physics->touches.fluid && physics->velocity.y > 0)
                    spawnFluidSplash(fluid.leaveSound, *physics, *bodyInFluid, fluid);

                constexpr float BUBBLES_SPAWN_CHANCE = 15;

                if (mu::random() < BUBBLES_SPAWN_CHANCE * deltaTime && fluid.bubbleSprite.isSet())
                {
                    float vel = length(physics->velocity);
                    int nrOfBubbles = vel * .015 * fluid.bubblesAmount;

                    for (int i = 0; i < nrOfBubbles; i++)
                    {
                        auto bubbleE = room->entities.create();
                        room->entities.assign<AABB>(bubbleE).center = bodyInFluid->randomPointInAABB();
                        room->entities.assign<AsepriteView>(bubbleE, fluid.bubbleSprite);
                        auto &p = room->entities.assign<Physics>(bubbleE);
                        p.ignoreFluids = false;
                        p.fluidAnimations = false;
                        p.gravity = fluid.reduceGravity * .5;
                        p.velocity = vec2(mu::random(-vel, vel), mu::random(-vel, vel)) * 1.5f;
                        room->entities.assign<FluidBubbleParticle>(bubbleE).timeAlive = mu::random(.2, .5);
                    }
                }
            }
        };
        updateEntitiesInFluid(fluid.entitiesInFluid);
        updateEntitiesInFluid(fluid.entitiesLeftFluid);
    });
}


void FluidsSystem::spawnFluidSplash(const asset<au::Sound> &sound, const Physics &physics, const AABB &aabb, const Fluid &fluid)
{
    float absYVel = abs(physics.velocity.y);

    auto speakerEntity = room->entities.create();
    auto &s = room->entities.assign<SoundSpeaker>(speakerEntity, sound);
    s.volume = min<float>(1., absYVel / 200);
    s.pitch = max<float>(.8, 2. - (absYVel / 200)) + mu::random(-.3, .3) ;
    room->entities.assign<DespawnAfter>(speakerEntity, .4f);

    for (int i = 0; i < absYVel * .1 * fluid.splatterAmount; i++)
    {
        auto dropE = room->entities.create();
        room->entities.assign<AABB>(dropE, ivec2(1), aabb.bottomCenter());
        auto &dropPhysics = room->entities.assign<Physics>(dropE);
        dropPhysics.airFriction = 0.;
        dropPhysics.velocity = rotate(vec2(0, absYVel * mu::random(.5, 1.6)), mu::random(20, 70) * mu::DEGREES_TO_RAD);
        if (mu::random() > .5)
            dropPhysics.velocity.x *= -1;
        dropPhysics.velocity.x -= physics.velocity.x;
        dropPhysics.velocity *= fluid.splatterVelocity;

        if (physics.velocity.y > 0)
            dropPhysics.velocity *= .6; // leaving fluid

        auto &drop = room->entities.assign<BloodDrop>(dropE);
        drop.size = mu::random(.2, 1.3 * fluid.splatterDropSize);
        drop.permanentDrawOnTerrain = false;
        drop.split = false;
        drop.color = fluid.color;

        room->entities.assign<DespawnAfter>(dropE, mu::random(.4, .6));

    }
}

