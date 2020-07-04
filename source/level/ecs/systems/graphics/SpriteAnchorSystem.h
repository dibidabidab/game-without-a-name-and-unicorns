
#ifndef GAME_SPRITEANCHORSYSTEM_H
#define GAME_SPRITEANCHORSYSTEM_H

#include "../../../room/Room.h"
#include "../EntitySystem.h"
#include "../../components/graphics/SpriteAnchor.h"
#include "../../components/graphics/AsepriteView.h"
#include "../../components/physics/Physics.h"

/**
 * see SpriteAnchor documentation
 */
class SpriteAnchorSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<SpriteAnchor, AABB>().each([&](
                const SpriteAnchor &anchor, AABB &aabb
        ) {

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
        });
    }
};

#endif
