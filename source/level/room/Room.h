
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <vector>
#include "../ecs/systems/EntitySystem.h"
#include "../../../entt/src/entt/entity/registry.hpp"
#include "TileMap.h"
#include <json.hpp>
#include <utils/base64.h>

class Room
{
    std::vector<EntitySystem *> systems;

    bool initialized = false;

    friend void from_json(const json& j, Room& r);

    TileMap *tileMap = NULL;

  public:

    entt::registry entities;

    Room() = default;

    Room(ivec2 size);

    ~Room();

    TileMap &getMap() const;

    void update(double deltaTime);

};

void to_json(json& j, const Room& r);

void from_json(const json& j, Room& r);

#endif
