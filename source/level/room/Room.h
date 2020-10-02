
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <ecs/EntityEngine.h>
#include "TileMap.h"
#include "../../generated/Saving.hpp"
#include <json.hpp>
#include <utils/base64.h>

class Level;

/**
 * A room is part of a level.
 *
 * A room has:
 * - entities
 * - entity systems
 * - a (tiled) terrain map
 */
class Room : public EntityEngine
{
    TileMap *tileMap = NULL;

    Level *level = NULL;
    int roomI = -1;

    friend void from_json(const json& j, Room& r);
    friend void to_json(json& j, const Room& r);
    friend void from_json(const json& j, Level& lvl);
    friend Level;

    json persistentEntitiesToLoad, revivableEntitiesToSave;

    void initialize(Level *lvl);

  protected:
    void initializeLuaEnvironment() override;

  private:

    void loadPersistentEntities();

  public:

    std::string name;
    uint baseLightLevel = 0u;
    uvec2 positionInLevel = uvec2(0);

    Room() = default;

    Room(ivec2 size);

    ~Room() override;

    void resize(int moveLeftBorder, int moveRightBorder, int moveTopBorder, int moveBottomBorder);

    TileMap &getMap() const;

    Level &getLevel() const { return *level; };

    int getIndexInLevel() const { return roomI; };

    int nrOfPersistentEntities() const;

    void update(double deltaTime) override;

  private:

    void persistentEntityToJson(entt::entity, const Persistent &, json &j) const;

    void tryToSaveRevivableEntity(entt::registry &, entt::entity);
};

void to_json(json& j, const Room& r);

void from_json(const json& j, Room& r);

#endif
