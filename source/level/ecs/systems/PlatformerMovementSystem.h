
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
    void update(double deltaTime, Room *room) override
    {
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
            input.jump = KeyInput::pressed(GLFW_KEY_SPACE);
            input.fall = KeyInput::pressed(GLFW_KEY_S);
            input.left = KeyInput::pressed(GLFW_KEY_A);
            input.right = KeyInput::pressed(GLFW_KEY_D);

            room->entities.assign_or_replace<PlatformerMovementInput>(e, input);
        });
        room->entities.view<PlatformerMovement, PlatformerMovementInput, Physics>()
            .each([&](entt::entity e, PlatformerMovement &movement, PlatformerMovementInput &input, Physics &physics) {

            if (input.jump && physics.touches.floor && physics.velocity.y <= 0) physics.velocity.y = movement.jumpVelocity;

            physics.velocity.x = ((input.left ? -1 : 0) + (input.right ? 1 : 0)) * movement.walkVelocity;

            if (physics.touches.halfSlopeDown || physics.touches.halfSlopeUp)
                physics.velocity.x /= 1.11803; // https://www.wolframalpha.com/input/?i=distance+between+%280%2C+0%29+and+%281%2C+0.5%29
            else if (physics.touches.slopeDown || physics.touches.slopeUp)
                physics.velocity.x /= 1.41421; // https://www.wolframalpha.com/input/?i=distance+between+%280%2C+0%29+and+%281%2C+1%29

            // ignore platforms when down-arrow is pressed:
            if (input.fall)
            {
                physics.ignorePlatforms = true;
                movement.fallPressedTimer = 0;
            }
            // ignore platforms for at least 0.2 sec since down-arrow was pressed:
            if (physics.ignorePlatforms && (movement.fallPressedTimer += deltaTime) > .1 && !input.fall)
                physics.ignorePlatforms = false;
        });
    }
};


#endif
