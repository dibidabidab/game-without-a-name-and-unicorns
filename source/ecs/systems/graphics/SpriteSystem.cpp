
#include "SpriteSystem.h"
#include "../../../generated/AsepriteView.hpp"
#include "../../../generated/SpriteAnchor.hpp"
#include "../../../generated/SpriteBobbing.hpp"
#include "../../../generated/Physics.hpp"

void SpriteSystem::update(double deltaTime, EntityEngine *room)
{
    this->room = room;

    updateFeetBobbing(deltaTime);
    updateAnimations(deltaTime);
    room->entities.view<SpriteAnchor, AABB>().each([&](auto &anchor, auto &aabb) {
        updateAnchor(anchor, aabb);
    });
}

void SpriteSystem::updateAnchor(const SpriteAnchor &anchor, AABB &aabb)
{
    AABB *spriteEntityAABB = room->entities.try_get<AABB>(anchor.spriteEntity);
    if (!spriteEntityAABB)
        return;

    AsepriteView *spriteView = room->entities.try_get<AsepriteView>(anchor.spriteEntity);
    if (!spriteView)
        return;

    auto &slice = spriteView->sprite->getSliceByName(anchor.spriteSliceName.c_str(), spriteView->frame);

    aabb.center = spriteView->getDrawPosition(*spriteEntityAABB);

    ivec2 sliceOffset(0);

    if (!spriteView->flipHorizontal || anchor.ignoreSpriteFlipping)
        sliceOffset.x += slice.originX;
    else
        sliceOffset.x += spriteView->sprite->width - slice.originX;

    if (!spriteView->flipVertical || anchor.ignoreSpriteFlipping)
        sliceOffset.y += spriteView->sprite->height - slice.originY;
    else
        sliceOffset.y += slice.originY;

    if (spriteView->rotate90Deg)
        sliceOffset = ivec2(sliceOffset.y, sliceOffset.x);

    aabb.center += sliceOffset;
}

void SpriteSystem::updateFeetBobbing(double deltaTime)
{
    room->entities.view<SpriteBobbing, AsepriteView, AABB>().each([&](
            auto e, SpriteBobbing &bobbing, AsepriteView &view, const AABB &body
    ) {

        float footHeight = 0;
        int n = 0;
        for (auto e : bobbing.feet)
        {
            const AABB *footAABB = room->entities.try_get<AABB>(e);
            if (footAABB)
            {
                n++;
                footHeight += footAABB->center.y;
            }
        }
        int avgFootHeight = round(footHeight / n);
        int diff = avgFootHeight - body.center.y + body.halfSize.y;

        Physics *physics = room->entities.try_get<Physics>(e);
        if (physics)
        {
            if (physics->touches.floor && !physics->prevTouched.floor)
                bobbing.floorHitVelocity = physics->prevVelocity.y * .6;
            if (!physics->touches.floor && bobbing.floorHitVelocity < 0)
                bobbing.floorHitVelocity = 0;

            bobbing.floorHitVelocity += 1700 * deltaTime;
            bobbing.floorHitYPos = min<float>(0, bobbing.floorHitYPos + bobbing.floorHitVelocity * deltaTime);

            diff += bobbing.floorHitYPos;
        }
        if (diff < bobbing.minYPos)
        {
            bobbing.floorHitVelocity = max(bobbing.floorHitVelocity, 0.f);
            diff = bobbing.minYPos;
        }
        if (diff > bobbing.maxYPos)
            diff = bobbing.maxYPos;

        view.positionOffset.y = diff;
    });
}

void SpriteSystem::updateAnimations(double deltaTime)
{
    room->entities.view<AsepriteView, AABB>().each([&](AsepriteView &view, const AABB &aabb) {

        if (!view.sprite.isSet())
            return;

        if (view.frame >= view.sprite->frameCount)
            view.frame = 0;

        if (int(view.playingTag) >= int(view.sprite->tags.size()))
            view.playingTag = -1;

        if (view.playingTag < 0)
            return;

        if (!view.paused)
            view.frameTimer += deltaTime;

        const auto *frame = &view.sprite->frames.at(view.frame);

        while (view.frameTimer >= frame->duration)
        {
            frame = &view.sprite->frames.at(view.frame);
            view.frameTimer -= frame->duration;

            const auto &tag = view.sprite->tags[view.playingTag];

            if (tag.loopDirection == aseprite::Tag::pingPong)
            {
                if (view.pong && view.frame-- <= tag.from)
                {
                    view.pong = false;
                    if (!view.loop)
                    {
                        view.playingTag = -1;
                        view.frame = tag.from;
                        return;
                    }
                    view.frame = tag.from == tag.to ? tag.from : tag.from + 1;
                } else if (!view.pong && view.frame++ >= tag.to)
                {
                    view.pong = true;
                    view.frame = tag.from == tag.to ? tag.from : tag.to - 1;
                }
            } else if (tag.loopDirection == aseprite::Tag::reverse)
            {
                if (view.frame-- <= tag.from)
                {
                    if (!view.loop)
                    {
                        view.playingTag = -1;
                        view.frame = tag.from;
                        return;
                    }
                    view.frame = tag.to;
                }
            } else if (view.frame++ >= tag.to)
            {
                if (!view.loop)
                {
                    view.playingTag = -1;
                    view.frame = tag.to;
                    return;
                }
                view.frame = tag.from;
            }
        }
    });
}
