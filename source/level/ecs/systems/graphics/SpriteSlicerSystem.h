
#ifndef GAME_SPRITESLICERSYSTEM_H
#define GAME_SPRITESLICERSYSTEM_H

#include <list>
#include "../EntitySystem.h"

struct AABB;
struct AsepriteView;

class SpriteSlicerSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
    Room *room = NULL;

  protected:

    std::list<AABB> aabbs;
    std::list<AsepriteView> views;

    void update(double deltaTime, Room *room) override;

    void slice(AABB &, AsepriteView &, int steps);

};


#endif
