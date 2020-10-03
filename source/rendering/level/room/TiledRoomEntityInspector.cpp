
#include "TiledRoomEntityInspector.h"
#include "../../../generated/Physics.hpp"
#include "../../../ecs/components/component_methods.h"
#include <game/dibidab.h>
#include <generated/Inspecting.hpp>

void TiledRoomEntityInspector::pickEntityGUI(const Camera *cam, DebugLineRenderer &lineRenderer)
{
    if (KeyInput::justPressed(GLFW_KEY_ESCAPE))
        pickEntity = false;

    vec2 p = cam->getCursorRayDirection() + cam->position;
    bool breakk = false;
    lineRenderer.axes(p, 10, vec3(1));

    reg.view<AABB>().each([&] (entt::entity e, AABB &box) {

        if (box.contains(p) && !breakk)
        {
            MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10, 10);
            breakk = true;

            draw(box, lineRenderer, mu::Y);

            ImGui::SetTooltip("#%d", int(e));

            if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT, 10))
            {
                reg.assign_or_replace<Inspecting>(e);
                pickEntity = false;
            }
            return;
        }
        else draw(box, lineRenderer, mu::X);
    });
}

void TiledRoomEntityInspector::moveEntityGUI(const Camera *cam, DebugLineRenderer &lineRenderer)
{
    if (KeyInput::justPressed(GLFW_KEY_ESCAPE))
        moveEntity = false;

    vec2 p = cam->getCursorRayDirection() + cam->position;
    bool breakk = false;

    auto drawSpawnPoint = [&] (auto currPos, auto spawnPos) {
        lineRenderer.axes(spawnPos, 10, mu::Y);
        lineRenderer.circle(spawnPos, 10, 10, mu::Y);
        lineRenderer.line(spawnPos, currPos, mu::Y);
    };

    if (movingEntity == entt::null)
    {
        lineRenderer.arrows(p, 10, vec3(1, 0, 1));

        reg.view<AABB>().each([&] (entt::entity e, AABB &box) {

            if (box.contains(p) && !breakk)
            {
                MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10);
                breakk = true;

                draw(box, lineRenderer, mu::Y);
                auto *persistent = reg.try_get<Persistent>(e);
                if (persistent && persistent->saveSpawnPosition)
                    drawSpawnPoint(p, persistent->spawnPosition);

                ImGui::SetTooltip("hold LMB to move #%d", int(e));

                if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT, 10))
                    movingEntity = e;
                return;
            }
            else draw(box, lineRenderer, mu::X);
        });

    }
    else
    {
        MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10, 5);
        lineRenderer.arrows(p, 5, vec3(0, 1, 0));
        auto *aabb = reg.try_get<AABB>(movingEntity);

        if (MouseInput::justReleased(GLFW_MOUSE_BUTTON_LEFT, 10) || aabb == NULL)
        {
            movingEntity = entt::null;
            moveEntity = false;
            return;
        }
        auto *persistent = reg.try_get<Persistent>(movingEntity);
        if (persistent && persistent->saveSpawnPosition)
        {
            drawSpawnPoint(p, persistent->spawnPosition);
            if (KeyInput::pressed(dibidab::settings.keyInput.moveEntityAndSpawnPoint))
                persistent->spawnPosition = vec3(p, 0);
            else
                ImGui::SetTooltip("Move SpawnPoint by holding %s", KeyInput::getKeyName(dibidab::settings.keyInput.moveEntityAndSpawnPoint));
        }

        auto *physics = reg.try_get<Physics>(movingEntity);
        if (physics && physics->gravity != 0)
            physics->velocity.y = 0;

        aabb->center = p;
    }
}
