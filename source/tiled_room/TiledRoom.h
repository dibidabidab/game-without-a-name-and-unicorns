
#ifndef GAME_TILEDROOM_H
#define GAME_TILEDROOM_H

#include <level/room/Room.h>
#include "TileMap.h"


/**
 * A room is part of a level.
 *
 * A room has:
 * - entities
 * - entity systems
 * - a (tiled) terrain map
 */
class TiledRoom : public Room
{
    TileMap *tileMap = NULL;
    WindMap *wind = NULL;

  protected:
    void initialize(Level *lvl) override;

    void initializeLuaEnvironment() override;

  public:

    float updateTimeMultiplier = 1., timeUntilTimeMultiplierReset = 0.;

    std::list<TileMap> decorativeTileLayers;

    uint baseLightLevel = 0u;
    uvec2 positionInLevel = uvec2(0);

    TiledRoom() = default;

    TiledRoom(ivec2 size);

    ~TiledRoom() override;

    void resize(int moveLeftBorder, int moveRightBorder, int moveTopBorder, int moveBottomBorder);

    TileMap &getMap() const;

    WindMap &getWindMap() const;

    void update(double deltaTime) override;

    vec3 getPosition(entt::entity) const override;

    void setPosition(entt::entity, const vec3 &) override;

    void toJson(json &) override;

    void fromJson(const json &) override;

};


#endif
