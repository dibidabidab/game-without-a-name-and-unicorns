
#ifndef GAME_SPRITESLICERSYSTEM_H
#define GAME_SPRITESLICERSYSTEM_H

#include <list>
#include <utils/math_utils.h>
#include <ecs/systems/EntitySystem.h>

struct AABB;
struct AsepriteView;
struct Physics;

class SpriteSlicerSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
    EntityEngine *room = NULL;

  protected:

    // code breaks if you change std::list to std::vector :)
    std::list<AABB> aabbs;
    std::list<AsepriteView> views;
    std::list<Physics> physicss;

    void update(double deltaTime, EntityEngine *room) override;

    void slice(Physics &, AABB &, AsepriteView &, int steps, const ivec2 &originalPosition);

    void setParticlePosition(AABB &aabb, AsepriteView &view, const ivec2 &originalPosition);
};


#endif
