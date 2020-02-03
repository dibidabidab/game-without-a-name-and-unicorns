
#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "Room.h"
#include "../../entt/src/entt/entity/registry.hpp"
#include "ecs/systems/LevelSystem.h"

class Level
{
    std::vector<LevelSystem *> systems;

    Room *currentRoom = nullptr;

    double time = 0;

  public:

    static const int PIXELS_PER_BLOCK = 16;

    entt::registry entities;

    Level();
    ~Level();

    Room *getCurrentRoom() { return currentRoom; }

    double getTime() const { return time; }

    /**
     * Updates the level and all it's LevelSystems.
     *
     * @param deltaTime Time passed since previous update
     */
    void update(double deltaTime);

};


#endif
