
#ifndef GAME_PLATFORMERMOVEMENTSYSTEM_H
#define GAME_PLATFORMERMOVEMENTSYSTEM_H


#include <input/key_input.h>
#include "LevelSystem.h"
#include "../../Level.h"
#include "../components/Physics.h"
#include "../components/PlatformerMovement.h"

class PlatformerMovementSystem : public LevelSystem
{
  protected:
    void update(double deltaTime, Level *lvl) override
    {
        lvl->entities.view<PlatformerMovement, LocalPlayer>().each([&](PlatformerMovement &movement, LocalPlayer) {
            movement.jump = KeyInput::pressed(GLFW_KEY_SPACE);
            movement.left = KeyInput::pressed(GLFW_KEY_LEFT);
            movement.right = KeyInput::pressed(GLFW_KEY_RIGHT);
            movement.fall = KeyInput::pressed(GLFW_KEY_DOWN);
        });
        lvl->entities.view<PlatformerMovement, Physics>().each([&](PlatformerMovement &movement, Physics &physics) {

            if (movement.jump && physics.touches.floor && physics.velocity.y <= 0) physics.velocity.y = movement.jumpVelocity;

            physics.velocity.x = ((movement.left ? -1 : 0) + (movement.right ? 1 : 0)) * movement.walkVelocity;

            physics.ignorePlatforms = movement.fall;
        });
    }
};


#endif
