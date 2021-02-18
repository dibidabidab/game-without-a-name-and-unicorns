
#ifndef GAME_SPRINKLERSYSTEM_H
#define GAME_SPRINKLERSYSTEM_H

#include <ecs/systems/EntitySystem.h>

class SprinklerSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:
    void update(double deltaTime, EntityEngine *engine) override;

};


#endif
