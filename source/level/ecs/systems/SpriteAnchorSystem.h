
#ifndef GAME_SPRITEANCHORSYSTEM_H
#define GAME_SPRITEANCHORSYSTEM_H

#include "../../room/Room.h"
#include "EntitySystem.h"
#include "../components/SpriteAnchor.h"
#include "../components/AsepriteView.h"
#include "../components/Physics.h"

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
            aabb.center.x += slice.originX;
            aabb.center.y += spriteView->sprite->height - slice.originY;
        });
    }
};

#endif
