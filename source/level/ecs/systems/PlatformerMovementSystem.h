
#ifndef GAME_PLATFORMERMOVEMENTSYSTEM_H
#define GAME_PLATFORMERMOVEMENTSYSTEM_H


#include <input/key_input.h>
#include "EntitySystem.h"
#include "../../Level.h"
#include "../components/Physics.h"
#include "../components/PlatformerMovement.h"
#include "../components/PlayerControlled.h"

class PlatformerMovementSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:
    void update(double deltaTime, Room *room) override
    {
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

            if (physics.touches.slopeUp && physics.velocity.x < 0)
                physics.velocity.y = physics.velocity.x;
            else if (physics.touches.slopeDown && physics.velocity.x > 0)
                physics.velocity.y = -physics.velocity.x;

            if (input.jump && physics.touches.floor && physics.velocity.y <= 0) physics.velocity.y = movement.jumpVelocity;

            physics.velocity.x = ((input.left ? -1 : 0) + (input.right ? 1 : 0)) * movement.walkVelocity;

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
