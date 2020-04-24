
#ifndef GAME_TERRAINCOLLISIONDETECTOR_H
#define GAME_TERRAINCOLLISIONDETECTOR_H

#include "../../../../macro_magic/serializable.h"

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
        rightWall         = false;
};

// TODO, bug: when Tile::slope_up and Tile::slope_down are placed next to each other -> player can fall through them
class TerrainCollisionDetector
{
    TileMap *map;

  public:
    explicit TerrainCollisionDetector(TileMap &map) : map(&map) {};

    /**
     * Detects collisions with the terrain.
     */
    TerrainCollisions detect(const AABB &aabb, bool ignorePlatforms);

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

};


#endif
