
#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "Room.h"
#include "../../entt/src/entt/entity/registry.hpp"

class Level
{
    entt::registry entities;

    Room *currentRoom = nullptr;

  public:

    Level();
    ~Level();

    Room *getCurrentRoom() { return currentRoom; }

};


#endif
