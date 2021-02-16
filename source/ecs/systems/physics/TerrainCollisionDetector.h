
#ifndef GAME_TERRAINCOLLISIONDETECTOR_H
#define GAME_TERRAINCOLLISIONDETECTOR_H

#include <utils/math_utils.h>
#include "../../../../external/entt/src/entt/entity/registry.hpp"
#include "../../../generated/TerrainCollisions.hpp"

class TileMap;
struct AABB;

typedef uint8 TileMaterial;

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
    void detect(TerrainCollisions &, const AABB &aabb, bool ignorePlatforms, bool ignorePolyPlatforms=true, bool ignoreFluids=true, uint autoStepHeight=0u);

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
