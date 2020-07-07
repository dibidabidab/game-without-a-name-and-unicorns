
#ifndef GAME_SPAWNINGSYSTEM_H
#define GAME_SPAWNINGSYSTEM_H

#include "EntitySystem.h"
#include "../components/Spawning.h"
#include "../../room/Room.h"


class SpawningSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    Room *room = NULL;

  protected:
    void update(double deltaTime, Room *room) override;

    void spawn(entt::entity spawnerEntity, TemplateSpawner &spawner);
};


#endif
