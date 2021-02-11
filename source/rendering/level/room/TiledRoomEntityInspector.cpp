
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

    entt::entity smallest = entt::null;
    AABB smallestAABB;

    reg.view<AABB>().each([&] (entt::entity e, AABB &box) {

        if (box.contains(p))
        {
            if (smallest == entt::null || length2(vec2(box.halfSize)) < length2(vec2(smallestAABB.halfSize)))
            {
                smallestAABB = box;
                smallest = e;
            }
        }
        draw(box, lineRenderer, mu::X);
    });
    if (smallest != entt::null)
    {
        MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10, 10);

        draw(smallestAABB, lineRenderer, mu::Y);

        if (auto name = engine.getName(smallest))
            ImGui::SetTooltip((std::string(name) + " #%d").c_str(), int(smallest));
        else
            ImGui::SetTooltip("#%d", int(smallest));

        if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT, 10))
        {
            reg.assign_or_replace<Inspecting>(smallest);
            pickEntity = false;
        }
    }
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

        entt::entity smallest = entt::null;
        AABB smallestAABB;

        reg.view<AABB>().each([&] (entt::entity e, AABB &box) {

            if (box.contains(p))
            {
                if (smallest == entt::null || length2(vec2(box.halfSize)) < length2(vec2(smallestAABB.halfSize)))
                {
                    smallestAABB = box;
                    smallest = e;
                }
            }
            draw(box, lineRenderer, mu::X);
        });

        if (smallest != entt::null)
        {
            MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10);
            breakk = true;

            draw(smallestAABB, lineRenderer, mu::Y);
            auto *persistent = reg.try_get<Persistent>(smallest);
            if (persistent && persistent->saveSpawnPosition)
                drawSpawnPoint(p, persistent->spawnPosition);

            ImGui::SetTooltip("hold LMB to move #%d", int(smallest));

            if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT, 10))
                movingEntity = smallest;
        }
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

void TiledRoomEntityInspector::highLightEntity(entt::entity entity, const Camera *camera, DebugLineRenderer &renderer)
{
    if (auto *aabb = reg.try_get<AABB>(entity))
        draw(*aabb, renderer, mu::X);
}
