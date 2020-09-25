
#include "SpriteSlicerSystem.h"
#include "../../../level/room/Room.h"
#include "../../../generated/AsepriteView.hpp"
#include "../../../generated/Spawning.hpp"
#include "../../../generated/Physics.hpp"

void SpriteSlicerSystem::update(double deltaTime, EntityEngine *room)
{
    this->room = room;

    room->entities.view<AsepriteView, SliceSpriteIntoPieces, AABB>()
    .each([&](auto e0, AsepriteView &view, SliceSpriteIntoPieces &s, AABB &aabb) {

        view.paused = true;

        aabbs.push_back(aabb);
        views.push_back(view);

        Physics *p = room->entities.try_get<Physics>(e0);
        if (p)
            physicss.push_back(*p);
        else
            physicss.emplace_back();

        slice(physicss.back(), aabbs.back(), views.back(), s.steps, ivec2(aabb.center + view.positionOffset));

        room->entities.remove<SliceSpriteIntoPieces>(e0);
        room->entities.remove<AsepriteView>(e0);
    });

    auto aabbIt = aabbs.begin();
    auto viewIt = views.begin();
    auto physicsIt = physicss.begin();

    while (aabbIt != aabbs.end())
    {
        auto e = room->entities.create();
        room->entities.assign<AABB>(e, *aabbIt);
        room->entities.assign<AsepriteView>(e, *viewIt);
        room->entities.assign<Physics>(e, *physicsIt);
        room->entities.assign<DespawnAfter>(e).time = mu::random(.3, 4);

        ++aabbIt;
        ++viewIt;
        ++physicsIt;
    }
    aabbs.clear();
    views.clear();
    physicss.clear();
}

void SpriteSlicerSystem::slice(Physics &physics0, AABB &aabb0, AsepriteView &view0, int steps, const ivec2 &originalPosition)
{
    int axis = mu::random() > .5 ? 0 : 1; // 0 = x, 1 = y

    auto width = axis == 0 ? view0.sprite->width : view0.sprite->height;

    aabbs.push_back(aabb0);
    AABB &aabb1 = aabbs.back();
    views.push_back(view0);
    AsepriteView &view1 = views.back();
    physicss.push_back(physics0);
    Physics &physics1 = physicss.back();
    physics1.velocity.x += mu::random(-300, 300);
    physics1.velocity.y += mu::random(-300, 300);

    if (axis == 0) // x
    {
        uint8
            minX0 = view0.clip.lowerLeft.x,
            maxX0 = width - view0.clip.lowerRight.x,
            x0Range = maxX0 - minX0,

            minX1 = view0.clip.topLeft.x,
            maxX1 = width - view0.clip.topRight.x,
            x1Range = maxX1 - minX1;

        uint8
            x0 = mu::randomInt(minX0, maxX0),
            x1 = mu::randomInt(minX1, maxX1);

        float
            yDiffBottom = view0.clip.lowerRight.y - view0.clip.lowerLeft.y,
            yDiffTop = view0.clip.topRight.y - view0.clip.topLeft.y,

            addBottom = x0Range == 0 ? 0 : (float(x0 - minX0) / float(x0Range)) * yDiffBottom,
            addTop = x1Range == 0 ? 0 : (float(x1 - minX1) / float(x1Range)) * yDiffTop;


        view0.clip.lowerLeft.y += addBottom;
        view0.clip.lowerLeft.x = x0;
        view0.clip.topLeft.y += addTop;
        view0.clip.topLeft.x = x1;

        view1.clip.lowerRight.y = view0.clip.lowerLeft.y;
        view1.clip.lowerRight.x = width - x0;
        view1.clip.topRight.y = view0.clip.topLeft.y;
        view1.clip.topRight.x = width - x1;
    }
    else // y
    {
        uint8
            minX0 = view0.clip.lowerLeft.y,
            maxX0 = width - view0.clip.topLeft.y,
            x0Range = maxX0 - minX0,

            minX1 = view0.clip.lowerRight.y,
            maxX1 = width - view0.clip.topRight.y,
            x1Range = maxX1 - minX1;

        uint8
            x0 = mu::randomInt(minX0, maxX0),
            x1 = mu::randomInt(minX1, maxX1);

        float
            xDiffLeft = view0.clip.topLeft.x - view0.clip.lowerLeft.x,
            xDiffRight = view0.clip.topRight.x - view0.clip.lowerRight.x,

            addLeft = x0Range == 0 ? 0 : (float(x0 - minX0) / float(x0Range)) * xDiffLeft,
            addRight = x1Range == 0 ? 0 : (float(x1 - minX1) / float(x1Range)) * xDiffRight;

        view0.clip.lowerLeft.x += addLeft;
        view0.clip.lowerLeft.y = x0;
        view0.clip.lowerRight.x += addRight;
        view0.clip.lowerRight.y = x1;

        view1.clip.topLeft.x = view0.clip.lowerLeft.x;
        view1.clip.topLeft.y = width - x0;
        view1.clip.topRight.x = view0.clip.lowerRight.x;
        view1.clip.topRight.y = width - x1;
    }
    setParticlePosition(aabb0, view0, originalPosition);
    setParticlePosition(aabb1, view1, originalPosition);
    if (--steps > 0)
    {
        slice(physics0, aabb0, view0, steps, originalPosition);
        slice(physics1, aabb1, view1, steps, originalPosition);
    }
}

void SpriteSlicerSystem::setParticlePosition(AABB &aabb, AsepriteView &view, const ivec2 &originalPosition)
{
    aabb.center = originalPosition;
    aabb.halfSize.x = view.sprite->width / 2;
    aabb.halfSize.y = view.sprite->height / 2;

    auto bl = aabb.bottomLeft() + ivec2(view.clip.lowerLeft);
    auto br = aabb.bottomRight() + ivec2(-view.clip.lowerRight.x, view.clip.lowerRight.y);

    auto tl = aabb.topLeft() + ivec2(view.clip.topLeft.x, -view.clip.topLeft.y);
    auto tr = aabb.topRight() - ivec2(view.clip.topRight);

    auto avg = bl + br + tl + tr;
    avg /= 4;

    view.positionOffset = originalPosition - avg;
    aabb.center = avg;
    aabb.halfSize = ivec2(1);
}


