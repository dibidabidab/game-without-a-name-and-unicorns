
#ifndef GAME_TERRAINCOLLISIONDETECTOR_H
#define GAME_TERRAINCOLLISIONDETECTOR_H

#include "../../../../macro_magic/serializable.h"
#include "../../../../../external/entt/src/entt/entity/registry.hpp"
#include "../../../room/TileMap.h"

class TileMap;
struct AABB;

struct TerrainCollisions
{
    bool
        floor             = false,  // any floor (solid block, sloped block, platform block, poly-platform)
        flatFloor         = false,  // flat floors (solid block, platform block)
        slopeDown         = false,  // Tile::slope_down, Tile::slope_down_half0 or Tile::slope_down_half1
        slopeUp           = false,  // Tile::slope_up, Tile::slope_up_half0 or Tile::slope_up_half1
        halfSlopeDown     = false,  // Tile::slope_down_half0 or Tile::slope_down_half1
        halfSlopeUp       = false,  // Tile::slope_up_half0 or Tile::slope_up_half1
        ceiling           = false,  // any ceiling (solid block, sloped block)
        slopedCeilingDown = false,  // only Tile::sloped_ceil_down
        slopedCeilingUp   = false,  // only Tile::sloped_ceil_up
        leftWall          = false,  // anything that should prevent an entity from going to the left
        rightWall         = false,  // anything that should prevent an entity from going to the right
        polyPlatform      = false,  // (possibly moving?)-platforms with multiple non-grid-based vertices
        fluid             = false,  // fluids, eg: a pool of water
        anything          = false;  // ANY of the above

    TileMaterial floorMaterial = 0;

    /**
     * if polyPlatform == true, then the following values will be set:
     *
     * 'polyPlatformEntity' is the PolyPlatform the entity is standing on
     * 'polyPlatformDeltaRight' is the difference in the platform's height 1 pixel to the right.
     * 'polyPlatformDeltaLeft' is the difference in the platform's height 1 pixel to the left.
     */
    entt::entity polyPlatformEntity = entt::null;
    int8 polyPlatformDeltaRight = 0, polyPlatformDeltaLeft = 0;

    /**
     * the 'abovePolyPlatformEntity' is set if a PolyPlatform was detected under the entity.
     * 'pixelsAbovePolyPlatform' is the number of pixels between the entity and the PolyPlatform.
     *
     * Not guaranteed to be set if the entity already touches a PolyPlatform.
     */
    entt::entity abovePolyPlatformEntity = entt::null;
    uint8 pixelsAbovePolyPlatform = 0;

    uint8 fluidDepth = 0;
    entt::entity fluidEntity = entt::null;
};

// TODO, bug: when Tile::slope_up and Tile::slope_down are placed next to each other -> player can fall through them
class TerrainCollisionDetector
{
    const TileMap *map;
    entt::registry *reg;

  public:

    static constexpr uint8 DETECT_POLY_PLATFORM_MARGIN = 32;

    TerrainCollisionDetector(const TileMap &map, entt::registry *reg=NULL) : map(&map), reg(reg) {};

    /**
     * Detects collisions with the terrain.
     */
    void detect(TerrainCollisions &, const AABB &aabb, bool ignorePlatforms, bool ignorePolyPlatforms=true, bool ignoreFluids=true);

  private:

    bool ceilingIntersection(const AABB &);
    bool slopedCeilingDownIntersection(const AABB &);
    bool slopedCeilingUpIntersection(const AABB &);
    bool slopeDownIntersection(const AABB &, TileMaterial &);
    bool slopeUpIntersection(const AABB &, TileMaterial &);
    bool halfSlopeDownIntersection(const AABB &, TileMaterial &);
    bool halfSlopeUpIntersection(const AABB &, TileMaterial &);
    bool floorIntersection(const AABB &, bool ignorePlatforms, TileMaterial &);
    bool leftWallIntersection(const AABB &);
    bool rightWallIntersection(const AABB &);

    bool onPolyPlatform(const AABB &, TerrainCollisions &, bool fallThrough);

    bool inFluid(const AABB &, TerrainCollisions &);

};


#endif
