
#ifndef GAME_PLAYERCONTROLSYSTEM_H
#define GAME_PLAYERCONTROLSYSTEM_H


#include "EntitySystem.h"

/**
 * responsible for noticing when a player enters or leaves a room.
 */
class PlayerControlSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:

    Room *room = NULL;

    void init(EntityEngine *r) override
    {
        room = (Room *) r;
        room->entities.on_construct<PlayerControlled>().connect<&PlayerControlSystem::onCreated>(this);
        room->entities.on_destroy<PlayerControlled>().connect<&PlayerControlSystem::onDestroyed>(this);
    }

    void onCreated(entt::registry &reg, entt::entity entity)
    {
        PlayerControlled &pC = reg.get<PlayerControlled>(entity);
        std::cout << "Entity (" << int(entity) << ") controlled by player "
            << int(pC.playerId) << " ENTERED Room " << room->getIndexInLevel() << "\n";

        room->getLevel().onPlayerEnteredRoom(room, pC.playerId);
    }

    void onDestroyed(entt::registry &reg, entt::entity entity)
    {
        PlayerControlled &pC = reg.get<PlayerControlled>(entity);
        std::cout << "Entity (" << int(entity) << ") controlled by player "
            << int(pC.playerId) << " LEFT Room" << room->getIndexInLevel() << "\n";

        room->getLevel().onPlayerLeftRoom(room, pC.playerId);
    }

    void update(double deltaTime, EntityEngine *room) override
    {

    }

    ~PlayerControlSystem() override
    {
        room->entities.view<PlayerControlled>().each([&](auto e, auto) {
            onDestroyed(room->entities, e);
        });
    }
};


#endif
