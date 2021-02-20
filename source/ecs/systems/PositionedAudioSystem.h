
#ifndef GAME_POSITIONEDAUDIOSYSTEM_H
#define GAME_POSITIONEDAUDIOSYSTEM_H

#include <ecs/systems/EntitySystem.h>

class PositionedAudioSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:
    void update(double deltaTime, EntityEngine *engine) override;


};


#endif
