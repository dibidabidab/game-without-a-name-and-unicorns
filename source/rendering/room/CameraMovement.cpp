
#include "CameraMovement.h"
#include "../../ecs/components/PlayerControlled.h"
#include "../../ecs/components/physics/Physics.h"
#include "../../ecs/components/combat/Aiming.h"

void move(float &x, float targetX, float deltaTime)
{
    float diff = abs(targetX - x);
    float step = 0;

    if (diff < 1)
        step = 0;
    else if (diff < 10)
        step = 60 * deltaTime;
    else
        step = 200 * deltaTime;

    x += step * (targetX - x < 0 ? -1.f : 1.f);
}

void move(double deltaTime, float &x, float targetX)
{
    float diff = targetX - x;
    float diffLength = abs(diff);

    float direction = diff < 0 ? -1 : 1;

    float step = direction * deltaTime * 20;

    float maxDiff = 6;

    if (diffLength > maxDiff)
        step = direction * (diffLength - maxDiff);

    if (abs(step) > diffLength)
        return;

    x += step;
}

void limit(float &x, float camWidth, float roomWidth)
{
    float minX = camWidth * .5;
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

    room->entities.view<LocalPlayer, AABB>().each([&](auto, AABB &aabb) {
        target = aabb.center;
    });

    move(deltaTime, cam->position.x, target.x);
    move(deltaTime, cam->position.y, target.y);

    limit(cam->position.x, cam->viewportWidth, room->getMap().width * TileMap::PIXELS_PER_TILE);
    limit(cam->position.y, cam->viewportHeight, room->getMap().height * TileMap::PIXELS_PER_TILE);

    vec2 halfSize(cam->viewportWidth * .5, cam->viewportHeight * .5);
    ivec2 bottomLeft = vec2(cam->position) - halfSize;
    cam->position = vec3(vec2(bottomLeft) + halfSize + offsetAnim, cam->position.z);

    {
        offsetAnim *= 1. - deltaTime * 6.;
//        float moveOffset = deltaTime * 120;
//
//        if (offsetAnim.x > 0)
//            offsetAnim.x = max<float>(0, offsetAnim.x - moveOffset);
//        if (offsetAnim.x < 0)
//            offsetAnim.x = min<float>(0, offsetAnim.x + moveOffset);
//        if (offsetAnim.y > 0)
//            offsetAnim.y = max<float>(0, offsetAnim.y - moveOffset);
//        if (offsetAnim.y < 0)
//            offsetAnim.y = min<float>(0, offsetAnim.y + moveOffset);
    }

    cam->update();
}
