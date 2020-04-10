
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
        room->entities.view<PlatformerMovement, PlayerControlled>().each([&](PlatformerMovement &movement, PlayerControlled) {
            movement.jump  = KeyInput::pressed(GLFW_KEY_SPACE);
            movement.left  = KeyInput::pressed(GLFW_KEY_A);
            movement.right = KeyInput::pressed(GLFW_KEY_D);
            movement.fall  = KeyInput::pressed(GLFW_KEY_S);
        });
        room->entities.view<PlatformerMovement, Physics>().each([&](PlatformerMovement &movement, Physics &physics) {

            if (movement.jump && physics.touches.floor && physics.velocity.y <= 0) physics.velocity.y = movement.jumpVelocity;

            physics.velocity.x = ((movement.left ? -1 : 0) + (movement.right ? 1 : 0)) * movement.walkVelocity;

            // ignore platforms when down-arrow is pressed:
            if (movement.fall)
            {
                physics.ignorePlatforms = true;
                movement.fallPressedTimer = 0;
            }
            // ignore platforms for at least 0.2 sec since down-arrow was pressed:
            if (physics.ignorePlatforms && (movement.fallPressedTimer += deltaTime) > .1 && !movement.fall)
                physics.ignorePlatforms = false;
        });
    }
};


#endif
