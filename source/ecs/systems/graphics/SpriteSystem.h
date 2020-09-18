
#ifndef GAME_SPRITESYSTEM_H
#define GAME_SPRITESYSTEM_H

#include "../../../level/room/Room.h"
#include "../EntitySystem.h"
#include "../../components/graphics/SpriteAnchor.h"
#include "../../../generated/AsepriteView.hpp"
#include "../../../generated/Physics.hpp"

/**
 * see SpriteAnchor documentation
 */
class SpriteSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    EntityEngine *room = NULL;

    void update(double deltaTime, EntityEngine *room) override;

    void updateAnchor(const SpriteAnchor &, AABB &);

    void updateFeetBobbing(double deltaTime);

    void updateAnimations(double deltaTime);
};

#endif
