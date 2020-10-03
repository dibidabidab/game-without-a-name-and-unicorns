
/**
 *
 * True ECS components should not have any methods (since logic should be done by systems).
 *
 * buttttttttttttt sometimes it's just very tempting to give components methods anyway, hence this file.
 *
 */


#include "../../generated/Leg.hpp"
#include "../../generated/Health.hpp"
#include "../../generated/AsepriteView.hpp"
#include "../../generated/Physics.hpp"

void Leg::stopMoving()
{
    moving = false;
    shouldBeMoving = false;
    distToTargetBeforeMoving = 0;
    timeSinceStartedMoving = 0;
    moveAccumulator = vec2(0);
    oppositeLegWaiting = 0;
}

bool Health::doesTakeDamageType(const std::string &damageType) const
{
    return takesDamageFrom.find(damageType) != takesDamageFrom.end();
}


float AsepriteView::playTag(const char *tagName, bool unpause)
{
    int i = 0;
    for (auto &tag : sprite->tags)
    {
        if (tag.name == tagName)
        {
            frame = tag.from;
            frameTimer = 0;
            pong = false;
            playingTag = i;
            paused = unpause ? false : paused;
            return tag.duration;
        }
        i++;
    }
    return 0;
}

ivec2 AsepriteView::getDrawPosition(const AABB &aabb) const
{
    ivec2 position = aabb.bottomLeft();
    position += positionOffset;
    position += originAlign * vec2(-sprite->width, -sprite->height);
    position += aabbAlign * vec2(aabb.halfSize * 2);
    return position;
}
