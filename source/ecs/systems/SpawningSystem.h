
#ifndef GAME_SPAWNINGSYSTEM_H
#define GAME_SPAWNINGSYSTEM_H

#include "EntitySystem.h"
#include "../components/Spawning.yaml"
#include "../../level/room/Room.h"


class SpawningSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

    EntityEngine *room = NULL;

  protected:
    void update(double deltaTime, EntityEngine *room) override;

    void spawn(entt::entity spawnerEntity, TemplateSpawner &spawner);
};


#endif
