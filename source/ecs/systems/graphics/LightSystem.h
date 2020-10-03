
#ifndef GAME_LIGHTSYSTEM_H
#define GAME_LIGHTSYSTEM_H

#include <ecs/systems/EntitySystem.h>

class LightSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *room) override;

};


#endif
