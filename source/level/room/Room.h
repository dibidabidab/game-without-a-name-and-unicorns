
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <list>
#include "ecs/entity_templates/EntityTemplate.h"
#include "ecs/systems/EntitySystem.h"
#include "../../../external/entt/src/entt/entity/registry.hpp"
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

    json persistentEntities;

    void initialize(Level *lvl);

    void loadPersistentEntities();

  public:

    std::string name;
    uint baseLightLevel = 0u;

    uvec2 positionInLevel = uvec2(0);

    ivec2 cursorPositionInRoom = ivec2(0);

    entt::registry entities;

    Room() = default;

    Room(ivec2 size);

    ~Room();

    void resize(int moveLeftBorder, int moveRightBorder, int moveTopBorder, int moveBottomBorder);

    TileMap &getMap() const;

    Level &getLevel() const { return *level; };

    int getIndexInLevel() const { return roomI; };

    int nrOfPersistentEntities() const;

    void update(double deltaTime);

    void addSystem(EntitySystem *sys);

    std::list<EntitySystem *> getSystems() { return systems; }

    template <class EntityTemplate_>
    EntityTemplate &getTemplate()
    {
        return getTemplate(getTypeName<EntityTemplate_>());
    }

    EntityTemplate &getTemplate(std::string name);

    EntityTemplate &getTemplate(int templateHash);

    const std::vector<std::string> &getTemplateNames() const;

    entt::entity getChildByName(entt::entity parent, const char *childName);

    template<typename Component>
    Component &getChildComponentByName(entt::entity parent, const char *childName)
    {
        entt::entity child = getChildByName(parent, childName);
        return entities.get<Component>(child);
    }

    template<typename Component>
    Component *tryGetChildComponentByName(entt::entity parent, const char *childName)
    {
        entt::entity child = getChildByName(parent, childName);
        return entities.try_get<Component>(child);
    }

  private:

    template <class EntityTemplate>
    void registerEntityTemplate()
    {
        auto name = getTypeName<EntityTemplate>();
        addEntityTemplate(name, new EntityTemplate());
    }

    void registerLuaEntityTemplate(const char *assetPath);

    void addEntityTemplate(const std::string &name, EntityTemplate *);

};

void to_json(json& j, const Room& r);

void from_json(const json& j, Room& r);

#endif
