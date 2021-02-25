
#include "FluidsSystem.h"
#include "../../../generated/BloodDrop.hpp"
#include "../../../generated/AsepriteView.hpp"
#include "../../components/component_methods.h"
#include "../PlatformerMovementSystem.h"
#include "../../../generated/Health.hpp"
#include <generated/SoundSpeaker.hpp>
#include <generated/Spawning.hpp>
#include <ecs/EntityEngine.h>

void FluidsSystem::update(double deltaTime, EntityEngine *room)
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

    room->entities.view<Fluid, AABB>().each([&](auto fluidE, Fluid &fluid, AABB &aabb) {

        auto updateEntitiesInFluid = [&](auto &entities) {
            for (auto e : entities)
            {
                Physics *physics_ = room->entities.try_get<Physics>(e);
                AABB *bodyInFluid_ = room->entities.try_get<AABB>(e);

                if (!physics_ || !bodyInFluid_ || !physics_->fluidAnimations)
                    continue;

                Physics physics = *physics_;
                AABB bodyInFluid = *bodyInFluid_;

                if (physics.touches.fluid && !physics.prevTouched.fluid && physics.velocity.y < 0)
                    spawnFluidSplash(fluid.enterSound, fluid, fluidE, physics, bodyInFluid);
                else if (physics.prevTouched.fluid && !physics.touches.fluid && physics.velocity.y > 0)
                    spawnFluidSplash(fluid.leaveSound, fluid, fluidE, physics, bodyInFluid);

                constexpr float BUBBLES_SPAWN_CHANCE = 15;

                if (mu::random() < BUBBLES_SPAWN_CHANCE * deltaTime && fluid.bubbleSprite.isSet())
                {
                    float vel = length(physics.velocity);
                    int nrOfBubbles = vel * .015 * fluid.bubblesAmount;

                    for (int i = 0; i < nrOfBubbles; i++)
                    {
                        auto bubbleE = room->entities.create();
                        room->entities.assign<AABB>(bubbleE).center = randomPointInAABB(bodyInFluid);
                        room->entities.assign<AsepriteView>(bubbleE).sprite = fluid.bubbleSprite;
                        auto &p = room->entities.assign<Physics>(bubbleE);
                        p.ignoreFluids = false;
                        p.fluidAnimations = false;
                        p.gravity = fluid.reduceGravity * .5;
                        p.velocity = vec2(mu::random(-vel, vel), mu::random(-vel, vel)) * 1.5f;
                        room->entities.assign<FluidBubbleParticle>(bubbleE).timeAlive = mu::random(.2, .5);
                    }
                }
            }

            if (fluid.damagePoints == 0)
                return;

            for (auto e : entities)
            {
                Health *health = room->entities.try_get<Health>(e);
                if (!health || health->immunityTimer > 0 || !health->doesTakeDamageType(fluid.damageType))
                    continue;

                auto &attack = health->attacks.emplace_back();
                attack.dealtBy = fluidE;
                attack.type = fluid.damageType;
                attack.points = fluid.damagePoints;
                attack.knockBackMultiplier = 0.;
            }
        };
        updateEntitiesInFluid(fluid.entitiesInFluid);
        updateEntitiesInFluid(fluid.entitiesLeftFluid);
    });
}

void FluidsSystem::spawnFluidSplash(const asset<au::Sound> &sound, const Fluid &fluid, entt::entity fluidE,
                                    const Physics &otherPhysics, const AABB &otherAABB)
{
    float sizeMultiplier = (otherAABB.halfSize.x * otherAABB.halfSize.y) / 10.f;
    sizeMultiplier = min(sizeMultiplier, 1.f);

    float absYVel = abs(otherPhysics.velocity.y);

    {   // SOUND:

        auto speakerEntity = room->entities.create();
        auto &s = room->entities.assign<SoundSpeaker>(speakerEntity);
        s.sound = sound;
        s.volume = min<float>(1., absYVel / 200) * sizeMultiplier;
        s.pitch = max<float>(.8, 2. - (absYVel / 200)) + mu::random(-.3, .3) ;
        room->entities.assign<PositionedAudio>(speakerEntity);
        room->entities.assign<AABB>(speakerEntity).center = otherAABB.center;
        room->entities.assign<DespawnAfter>(speakerEntity).time = 4;
    }


    // DROPS:
    for (int i = 0; i < absYVel * .1 * fluid.splatterAmount * sizeMultiplier; i++)
    {
        auto dropE = room->entities.create();
        room->entities.assign<AABB>(dropE).center = otherAABB.bottomCenter();
        auto &dropPhysics = room->entities.assign<Physics>(dropE);
        dropPhysics.airFriction = 0.;
        dropPhysics.velocity = rotate(vec2(0, absYVel * mu::random(.5, 1.6)), mu::random(20, 70) * mu::DEGREES_TO_RAD);
        if (mu::random() > .5)
            dropPhysics.velocity.x *= -1;
        dropPhysics.velocity.x -= otherPhysics.velocity.x;
        dropPhysics.velocity *= fluid.splatterVelocity;

        if (otherPhysics.velocity.y > 0)
            dropPhysics.velocity *= .6; // leaving fluid

        auto &drop = room->entities.assign<BloodDrop>(dropE);
        drop.size = mu::random(.2, 1.3 * fluid.splatterDropSize);
        drop.permanentDrawOnTerrain = false;
        drop.split = false;
        drop.color = fluid.color;

        room->entities.assign<DespawnAfter>(dropE).time = mu::random(.4, .6);
    }

    // WAVES:
    PolylineWaves *waves = room->entities.try_get<PolylineWaves>(fluidE);
    if (waves && waves->springs.size() > otherPhysics.touches.fluidSurfaceLineXIndex)
    {
        waves->springs[otherPhysics.touches.fluidSurfaceLineXIndex].velocity
            += otherPhysics.velocity.y * waves->impactMultiplier * sizeMultiplier;
    }
}

