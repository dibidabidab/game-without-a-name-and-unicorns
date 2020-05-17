
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <list>
#include "../ecs/entity_templates/EntityTemplate.h"
#include "../ecs/systems/EntitySystem.h"
#include "../../../entt/src/entt/entity/registry.hpp"
#include "TileMap.h"
#include "../Level.h"
#include <json.hpp>
#include <map>
#include <utils/base64.h>
#include <utils/type_name.h>
#include <utils/hashing.h>

class Level;

/**
 * A room is part of a level.
 *
 * A room has:
 * - entities
 * - entity systems
 * - a (tiled) terrain map
 */
class Room
{
    std::list<EntitySystem *> systems;
    std::map<int, EntityTemplate *> entityTemplates;
    std::vector<std::string> entityTemplateNames;

    bool initialized = false;

    TileMap *tileMap = NULL;

    Level *level = NULL;
    int roomI = -1;

    friend void from_json(const json& j, Room& r);
    friend void from_json(const json& j, Level& lvl);
    friend Level;

    void initialize(Level *lvl, int roomI);

  public:

    ivec2 cursorPositionInRoom = ivec2(0);

    entt::registry entities;

    Room() = default;

    Room(ivec2 size);

    ~Room();

    TileMap &getMap() const;

    Level *getLevel() const { return level; };

    int getIndexInLevel() const { return roomI; };

    void update(double deltaTime);

    void addSystem(EntitySystem *sys);

    template <class EntityTemplate>
    void registerEntityTemplate()
    {
        auto name = getTypeName<EntityTemplate>();
        int hash = hashStringCrossPlatform(name);
        auto et = entityTemplates[hash] = new EntityTemplate();
        et->room = this;
        et->templateHash = hash;
        entityTemplateNames.push_back(name);
    }

    template <class EntityTemplate_>
    EntityTemplate *getTemplate()
    {
        return getTemplate(getTypeName<EntityTemplate_>());
    }

    EntityTemplate *getTemplate(std::string name);

    EntityTemplate *getTemplate(int templateHash);

    const std::vector<std::string> &getTemplateNames() const;
};

void to_json(json& j, const Room& r);

void from_json(const json& j, Room& r);

#endif
