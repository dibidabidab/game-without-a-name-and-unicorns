
#ifndef GAME_FIRESYSTEM_H
#define GAME_FIRESYSTEM_H

#include <ecs/systems/EntitySystem.h>

class FireSystem : public EntitySystem
{

    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *engine) override;

};


#endif
