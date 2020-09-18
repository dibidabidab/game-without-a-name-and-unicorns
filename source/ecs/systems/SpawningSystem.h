
#ifndef GAME_SPAWNINGSYSTEM_H
#define GAME_SPAWNINGSYSTEM_H

#include "EntitySystem.h"
#include "../../level/room/Room.h"
#include "../../generated/Spawning.hpp"


class SpawningSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    EntityEngine *room = NULL;

  protected:
    void update(double deltaTime, EntityEngine *room) override;

    void spawn(entt::entity spawnerEntity, TemplateSpawner &spawner);
};


#endif
