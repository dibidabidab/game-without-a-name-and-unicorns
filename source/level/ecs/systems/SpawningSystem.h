
#ifndef GAME_SPAWNINGSYSTEM_H
#define GAME_SPAWNINGSYSTEM_H

#include "EntitySystem.h"
#include "../components/Spawning.h"
#include "../../room/Room.h"


class SpawningSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:
    void update(double deltaTime, Room *room) override;
};


#endif
