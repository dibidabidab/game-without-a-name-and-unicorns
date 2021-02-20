
#include "CameraMovement.h"
#include "../../../generated/Physics.hpp"
#include "../../../game/Game.h"
#include "../../../generated/CameraShaking.hpp"
#include <generated/PlayerControlled.hpp>
#include <game/dibidab.h>
#include <imgui.h>

void move(double deltaTime, float &x, float targetX)
{
    float diff = targetX - x;

    static const int IGNORE_DIFF = 10;

    if (diff > 0)
        diff = max(.0f, diff - IGNORE_DIFF);
    else diff = min(.0f, diff + IGNORE_DIFF);

    x += diff * deltaTime * 4;
}


static const int PADDING_X = 20, PADDING_Y = 16;

void limit(float &x, float camWidth, float roomWidth, float padding)
{
    float minX = camWidth * .5 - padding;
    float maxX = roomWidth - minX;

    if (minX > maxX)
    {
        x = roomWidth * .5;
        return;
    }
    x = std::min(x, maxX);
    x = std::max(x, minX);
}

void CameraMovement::update(double deltaTime)
{
    vec2 target = vec2(cam->position.x, cam->position.y);

    vec2 shakeOffset(0);

    room->entities.view<CameraShaking, AABB>().each([&](CameraShaking &shake, AABB &aabb) {

        shake.accumulator += deltaTime;
        const static float FREQ = 1./30.;
        if (shake.accumulator < FREQ)
            return;
        shake.accumulator -= FREQ;

        float intensity = shake.intensity;
        intensity *= 1. - min(1.f, length(vec2(aabb.center) - target) / shake.maxDistanceFromCamera);

        shakeOffset.x += mu::random(-intensity, intensity);
        shakeOffset.y += mu::random(-intensity, intensity);
    });
    maxShake.x = max(maxShake.x, abs(shakeOffset.x) * 20.f);
    maxShake.y = max(maxShake.y, abs(shakeOffset.y) * 20.f);

    room->entities.view<LocalPlayer, AABB>().each([&](auto, AABB &aabb) {
        target = aabb.center;
    });

    move(deltaTime, cam->position.x, target.x);
    move(deltaTime, cam->position.y, target.y);

    limit(cam->position.x, cam->viewportWidth, room->getMap().width * TileMap::PIXELS_PER_TILE, PADDING_X + maxShake.x);
    limit(cam->position.y, cam->viewportHeight, room->getMap().height * TileMap::PIXELS_PER_TILE, PADDING_Y + maxShake.y);

    vec2 halfSize(cam->viewportWidth * .5, cam->viewportHeight * .5);
    ivec2 bottomLeft = vec2(cam->position) - halfSize;
    cam->position = vec3(vec2(bottomLeft) + halfSize + offsetAnim + shakeOffset, cam->position.z);

    offsetAnim *= max(0., 1. - deltaTime * 6.);
    maxShake *= max(0., 1. - deltaTime * 6.);

    if (dibidab::settings.showDeveloperOptions)
    {
        if (MouseInput::pressed(GLFW_MOUSE_BUTTON_MIDDLE))
        {
            if (devCamPos == ivec2(-1))
                devCamPos = ivec2(cam->position);

            static vec2 accumulator(0);

            accumulator.x -= float(MouseInput::deltaMouseX);
            accumulator.y += float(MouseInput::deltaMouseY);

            devCamPos += accumulator / vec2(Game::settings.graphics.pixelScaling);
            accumulator -= floor(accumulator);
        }
        if (devCamPos != ivec2(-1))
        {
            cam->position.x = devCamPos.x + fract(cam->position.x); // fract() to fix pixel art issues.
            cam->position.y = devCamPos.y + fract(cam->position.y);
            ImGui::BeginMainMenuBar();
            if (ImGui::Button("Reset camera position"))
                devCamPos = ivec2(-1);
            ImGui::EndMainMenuBar();
        }
    }

    cam->update();
}
