
#ifndef GAME_SPRITESLICERSYSTEM_H
#define GAME_SPRITESLICERSYSTEM_H

#include "../EntitySystem.h"

struct AABB;
struct AsepriteView;

class SpriteSlicerSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
    Room *room = NULL;

  protected:
    void update(double deltaTime, Room *room) override;

    void slice(AABB &, AsepriteView &, int steps);

};


#endif
