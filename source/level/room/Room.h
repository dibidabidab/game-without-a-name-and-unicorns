
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <list>
#include "../ecs/systems/EntitySystem.h"
#include "../../../entt/src/entt/entity/registry.hpp"
#include "TileMap.h"
#include "../Level.h"
#include <json.hpp>
#include <utils/base64.h>

class Level;

class Room
{
    std::list<EntitySystem *> systems;

    bool initialized = false;

    TileMap *tileMap = NULL;

    Level *level = NULL;
    int roomI = -1;

    friend void from_json(const json& j, Room& r);
    friend void from_json(const json& j, Level& lvl);
    friend Level;

    void initialize(Level *lvl, int roomI);

  public:

    entt::registry entities;

    Room() = default;

    Room(ivec2 size);

    ~Room();

    TileMap &getMap() const;

    Level *getLevel() const { return level; };

    int getIndexInLevel() const { return roomI; };

    void update(double deltaTime);

    void addSystem(EntitySystem *sys);
};

void to_json(json& j, const Room& r);

void from_json(const json& j, Room& r);

#endif
