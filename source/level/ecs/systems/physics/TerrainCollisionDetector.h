
#ifndef GAME_TERRAINCOLLISIONDETECTOR_H
#define GAME_TERRAINCOLLISIONDETECTOR_H

#include "../../../../macro_magic/serializable.h"
#include "../../../../../entt/src/entt/entity/registry.hpp"

class TileMap;
struct AABB;

struct TerrainCollisions
{
    bool
        floor             = false,
        flatFloor         = false,
        slopeDown         = false,
        slopeUp           = false,
        halfSlopeDown     = false,
        halfSlopeUp       = false,
        ceiling           = false,
        slopedCeilingDown = false,
        slopedCeilingUp   = false,
        leftWall          = false,
        rightWall         = false,
        polyPlatform      = false,
        anything          = false;

    entt::entity polyPlatformEntity = entt::null;
    int8 polyPlatformDeltaRight = 0, polyPlatformDeltaLeft = 0;
};

// TODO, bug: when Tile::slope_up and Tile::slope_down are placed next to each other -> player can fall through them
class TerrainCollisionDetector
{
    const TileMap *map;
    entt::registry *reg;

  public:
    explicit TerrainCollisionDetector(const TileMap &map, entt::registry *reg=NULL) : map(&map), reg(reg) {};

    /**
     * Detects collisions with the terrain.
     */
    TerrainCollisions detect(const AABB &aabb, bool ignorePlatforms, bool ignorePolyPlatforms=true);

  private:

    bool ceilingIntersection(const AABB &aabb);
    bool slopedCeilingDownIntersection(const AABB &aabb);
    bool slopedCeilingUpIntersection(const AABB &aabb);
    bool slopeDownIntersection(const AABB &aabb);
    bool slopeUpIntersection(const AABB &aabb);
    bool halfSlopeDownIntersection(const AABB &aabb);
    bool halfSlopeUpIntersection(const AABB &aabb);
    bool floorIntersection(const AABB &aabb, bool ignorePlatforms);
    bool leftWallIntersection(const AABB &aabb);
    bool rightWallIntersection(const AABB &aabb);

    bool onPolyPlatform(const AABB &aabb, entt::entity &platformEntity, int8 &deltaLeft, int8 &deltaRight, bool fallThrough);

};


#endif
