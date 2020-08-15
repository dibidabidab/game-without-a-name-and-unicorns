
#ifndef GAME_PLATFORMERMOVEMENTSYSTEM_H
#define GAME_PLATFORMERMOVEMENTSYSTEM_H


#include <input/key_input.h>
#include "EntitySystem.h"
#include "../../Level.h"
#include "../components/physics/Physics.h"
#include "../components/PlatformerMovement.h"
#include "../components/PlayerControlled.h"
#include "../components/combat/Aiming.h"
#include "../components/graphics/AsepriteView.h"
#include "../components/Spawning.h"
#include "../components/SoundSpeaker.h"
#include "../../../Game.h"

/**
 * Responsible for:
 *  - setting the velocity of an entity (e.g. the player) based on (keyboard or AI) input.
 *  - setting the aim-target for the local player.
 *  - flipping entities+sprites when the entity aims to the left
 */
class PlatformerMovementSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:

    Room *room = NULL;

    void update(double deltaTime, Room *room) override
    {
        this->room = room;
        room->entities.view<LocalPlayer, Aiming>().each([&](auto, Aiming &aiming) {
            aiming.target = room->cursorPositionInRoom;
        });
        room->entities.view<Flip, Aiming, AABB>().each([&](Flip &flip, const Aiming &aiming, const AABB &aabb) {

            flip.horizontal = aiming.target.x < aabb.center.x;

        });
        room->entities.view<Flip, AsepriteView>().each([&](const Flip &flip, AsepriteView &view) {

            view.flipHorizontal = flip.horizontal;
            view.flipVertical = flip.vertical;

        });
        room->entities.view<PlatformerMovement, LocalPlayer>().each([&](entt::entity e, PlatformerMovement &movement, LocalPlayer) {

            PlatformerMovementInput input;
            input.jump = KeyInput::pressed(Game::settings.keyInput.jump);
            input.fall = KeyInput::pressed(Game::settings.keyInput.fall);
            input.left = KeyInput::pressed(Game::settings.keyInput.walkLeft);
            input.right = KeyInput::pressed(Game::settings.keyInput.walkRight);
            input.attack = MouseInput::pressed(GLFW_MOUSE_BUTTON_LEFT);

            room->entities.assign_or_replace<PlatformerMovementInput>(e, input);
        });
        room->entities.view<PlatformerMovement, PlatformerMovementInput, Physics, AABB>()
            .each([&](entt::entity e, PlatformerMovement &movement, PlatformerMovementInput &input, Physics &physics, const AABB &aabb) {

            if (
                    (
                            physics.touches.floor && !physics.prevTouched.floor
                            && physics.prevVelocity.y <= 0 // only stop the jump if player FELL on the floor (and not jumped through platform)
                    )
                    || physics.touches.fluid
            )
            {
                // set this to false BEFORE possibly jumping again in the next IF.
                movement.currentlyJumping = false;
                movement.jumpPressedSinceBegin = false;
            }

            if (
                    input.jump
                    && !movement.currentlyJumping
            )
            {
                bool touchesFloorOrWall =
                        physics.touches.fluid
                            ? (physics.touches.leftWall || physics.touches.rightWall) && physics.touches.fluidDepth - aabb.halfSize.y < 16
                            : (physics.touches.floor || physics.airTime < movement.coyoteTime) && physics.prevVelocity.y <= 0;

                if (touchesFloorOrWall)
                {
                    if (physics.touches.fluid)
                        physics.velocity.y = 0;
                    physics.velocity.y = max(movement.jumpVelocity, min(movement.maxJumpVelocity, movement.jumpVelocity + physics.velocity.y));
                    movement.jumpPressedSinceBegin = true;
                    movement.currentlyJumping = true;
                    if (physics.touches.floor)
                        spawnDustTrail(aabb.bottomCenter(), "jump");
                }
            }
            if (!input.jump)
                movement.jumpPressedSinceBegin = false;

            if (movement.jumpPressedSinceBegin && physics.velocity.y >= 0)
                physics.velocity.y += physics.gravity * movement.jumpAntiGravity * deltaTime;

            if (physics.touches.fluid && input.jump && physics.velocity.y < movement.swimVelocity)
                physics.velocity.y += movement.swimVelocity * deltaTime * min(1.f, physics.touches.fluidDepth / 16.f);

            float accelerateX = ((input.left ? -1 : 0) + (input.right ? 1 : 0)) * movement.walkVelocity;

            if (physics.touches.halfSlopeDown || physics.touches.halfSlopeUp)
                accelerateX /= 1.11803; // https://www.wolframalpha.com/input/?i=distance+between+%280%2C+0%29+and+%281%2C+0.5%29
            else if (physics.touches.slopeDown || physics.touches.slopeUp)
                accelerateX /= 1.41421; // https://www.wolframalpha.com/input/?i=distance+between+%280%2C+0%29+and+%281%2C+1%29

            if (physics.touches.floor)
                accelerateX *= physics.floorFriction;
            else
            {
                accelerateX *= physics.airFriction;
                accelerateX *= movement.airControl;    // faster acceleration in air depending on airControl.
            }
                accelerateX *= deltaTime;

            float accelerationProgress = abs(accelerateX) / (movement.walkVelocity - abs(physics.velocity.x)); // TODO: divide by 0?
            if (accelerationProgress > 1)
                accelerateX /= accelerationProgress;

            physics.velocity.x += accelerateX;

            // ignore platforms when down-arrow is pressed:
            if (input.fall && !physics.ignorePlatforms)
            {
                physics.ignorePlatforms = true;
                movement.fallPressedTimer = 0;
            }
            // ignore platforms for at least 0.1 sec since down-arrow was pressed:
            if (physics.ignorePlatforms && (movement.fallPressedTimer += deltaTime) > .1 && !input.fall)
                physics.ignorePlatforms = false;

            if (physics.touches.floor && !physics.prevTouched.floor)
            {
                if (physics.prevVelocity.y < -160)
                    spawnDustTrail(aabb.bottomCenter(), "land");
                if (physics.prevVelocity.y < -50)
                    spawnLandingSpeaker(physics.prevVelocity.y);
            }
        });
    }

    void spawnDustTrail(const ivec2 &position, const char *animationName)
    {
        entt::entity e = room->entities.create();

        room->entities.assign<AABB>(e, ivec2(3), position);
        auto &view = room->entities.assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/dust_trails"));
        view.originAlign.y = 0;

        float duration = view.playTag(animationName);

        room->entities.assign<DespawnAfter>(e, duration);
    }

    void spawnLandingSpeaker(float velocity)
    {
        entt::entity e = room->entities.create();

        auto &s = room->entities.assign<SoundSpeaker>(e, asset<au::Sound>("sounds/landing"));
        s.pitch = 2.f - std::min(1.f, std::max(0.f, velocity * -.0025f));

        s.volume = std::min(1.f, std::max(0.f, velocity * -.0003f));

        room->entities.assign<DespawnAfter>(e, 1.f);
    }

};


#endif
