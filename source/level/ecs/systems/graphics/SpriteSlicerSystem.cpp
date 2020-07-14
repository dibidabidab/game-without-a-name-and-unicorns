
#include "SpriteSlicerSystem.h"
#include "../../../room/Room.h"
#include "../../components/graphics/AsepriteView.h"

void SpriteSlicerSystem::update(double deltaTime, Room *room)
{
    this->room = room;

    room->entities.view<AsepriteView, SliceSpriteIntoPieces, AABB>()
    .each([&](auto e0, AsepriteView &view, SliceSpriteIntoPieces &s, AABB &aabb) {

        view.paused = true;
        slice(aabb, view, s.steps);

        room->entities.remove<SliceSpriteIntoPieces>(e0);
    });

    auto aabbIt = aabbs.begin();
    auto viewIt = views.begin();

    while (aabbIt != aabbs.end())
    {
        auto e = room->entities.create();
        room->entities.assign<AABB>(e, *aabbIt);
        room->entities.assign<AsepriteView>(e, *viewIt);

        ++aabbIt;
        ++viewIt;
    }
    aabbs.clear();
    views.clear();
}

void SpriteSlicerSystem::slice(AABB &aabb0, AsepriteView &view0, int steps)
{
    int axis = mu::random() > .5 ? 0 : 1; // 0 = x, 1 = y

    auto width = axis == 0 ? view0.sprite->width : view0.sprite->height;

    aabbs.push_back(aabb0);
    AABB &aabb1 = aabbs.back();
    views.push_back(view0);
    AsepriteView &view1 = views.back();

    if (axis == 0) // x
    {
        uint8
            minX0 = view0.clip.lowerLeft[axis],
            maxX0 = width - view0.clip.lowerRight[axis],
            x0Range = maxX0 - minX0,

            minX1 = view0.clip.topLeft[axis],
            maxX1 = width - view0.clip.topRight[axis],
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
            minX0 = view0.clip.lowerLeft[axis],
            maxX0 = width - view0.clip.topLeft[axis],
            x0Range = maxX0 - minX0,

            minX1 = view0.clip.lowerRight[axis],
            maxX1 = width - view0.clip.topRight[axis],
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
    if (--steps > 0)
    {
        slice(aabb0, view0, steps);
        slice(aabb1, view1, steps);
    }
}
