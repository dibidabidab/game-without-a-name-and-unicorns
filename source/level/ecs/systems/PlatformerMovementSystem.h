
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

            if (KeyInput::justPressed(GLFW_KEY_SPACE))
                room->entities.assign_or_replace<JumpInput>(e);
            if (KeyInput::justReleased(GLFW_KEY_SPACE))
                room->entities.remove_if_exists<JumpInput>(e);

            if (KeyInput::justPressed(GLFW_KEY_S))
                room->entities.assign_or_replace<FallInput>(e);
            if (KeyInput::justReleased(GLFW_KEY_S))
                room->entities.remove_if_exists<FallInput>(e);

            auto walkInput = room->entities.try_get<WalkInput>(e);

            if (!walkInput)
            {
                room->entities.assign<WalkInput>(e);
                return;
            }
            walkInput->left = KeyInput::pressed(GLFW_KEY_A);
            walkInput->right = KeyInput::pressed(GLFW_KEY_D);
        });
        room->entities.view<PlatformerMovement, Physics>().each([&](entt::entity e, PlatformerMovement &movement, Physics &physics) {

            auto walkInput = room->entities.try_get<WalkInput>(e);
            if (!walkInput)
                return;

            bool left = walkInput->left;
            bool right =  walkInput->right;
            bool jump = !!room->entities.try_get<JumpInput>(e);
            bool fall = !!room->entities.try_get<JumpInput>(e);

            if (jump && physics.touches.floor && physics.velocity.y <= 0) physics.velocity.y = movement.jumpVelocity;

            physics.velocity.x = ((left ? -1 : 0) + (right ? 1 : 0)) * movement.walkVelocity;

            // ignore platforms when down-arrow is pressed:
            if (fall)
            {
                physics.ignorePlatforms = true;
                movement.fallPressedTimer = 0;
            }
            // ignore platforms for at least 0.2 sec since down-arrow was pressed:
            if (physics.ignorePlatforms && (movement.fallPressedTimer += deltaTime) > .1 && !fall)
                physics.ignorePlatforms = false;
        });
    }
};


#endif
