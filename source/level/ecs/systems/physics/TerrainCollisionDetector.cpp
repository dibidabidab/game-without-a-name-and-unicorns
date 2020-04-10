
#include "TerrainCollisionDetector.h"
#include "../../components/Physics.h"
#include "../../../Level.h"

TerrainCollisions TerrainCollisionDetector::detect(const AABB &aabb, bool ignorePlatforms)
{
    TerrainCollisions collisions;

    collisions.slopeDown = slopeDownIntersection(aabb);
    collisions.slopeUp = slopeUpIntersection(aabb);
    collisions.floor = collisions.slopeUp || collisions.slopeDown || floorIntersection(aabb, ignorePlatforms);
    collisions.slopedCeilingDown = slopedCeilingDownIntersection(aabb);
    collisions.slopedCeilingUp = slopedCeilingUpIntersection(aabb);
    collisions.ceiling = collisions.slopedCeilingUp || collisions.slopedCeilingDown || ceilingIntersection(aabb);
    collisions.leftWall = collisions.slopedCeilingUp || leftWallIntersection(aabb);
    collisions.rightWall = collisions.slopedCeilingDown || rightWallIntersection(aabb);

    return collisions;
}

/**
 * Checks if the line from p0 to p1 intersects with Tile::full
 * @param p0    Should have smallest x and y
 * @param p1    Should have biggest x and y
 */
bool lineIntersectsWithFullBlock(const ivec2 &p0, const ivec2 &p1, TileMap *map)
{
    bool col = false;
    map->loopThroughTiles(p0, p1, [&](ivec2 t, Tile tile) {
       if (tile == Tile::full) col = true;
    });
    return col;
}

/**
 * Checks if point p overlaps a slope
 * @param slopeType     Type of slope to check for
 */
bool pointOnSlope(const ivec2 &p, TileMap *map, Tile slopeType)
{
    int
        tileX = p.x / TileMap::PIXELS_PER_TILE,
        tileY = p.y / TileMap::PIXELS_PER_TILE;
    int
        tilePixelX = p.x % TileMap::PIXELS_PER_TILE,
        tilePixelY = p.y % TileMap::PIXELS_PER_TILE;

    Tile tile = map->getTile(tileX, tileY);

    if (tile == slopeType) switch (tile)
    {
        case Tile::slope_down:
            return tilePixelY <= TileMap::PIXELS_PER_TILE - 1 - tilePixelX;

        case Tile::slope_up:
            return tilePixelY <= tilePixelX;

        case Tile::sloped_ceil_down:
            return tilePixelY >= TileMap::PIXELS_PER_TILE - 1 - tilePixelX;

        case Tile::sloped_ceil_up:
            return tilePixelY >= tilePixelX;
        default: break;
    }
    return false;
}

bool TerrainCollisionDetector::ceilingIntersection(const AABB &aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.center + aabb.halfSize * ivec2(-1, 1) + ivec2(1, 0),
            aabb.center + aabb.halfSize                - ivec2(1, 0),
            map
    );
}

bool TerrainCollisionDetector::slopeDownIntersection(const AABB &aabb)
{
    return
        pointOnSlope(aabb.bottomLeft() + ivec2(1, 0), map, Tile::slope_down)
        ||
        pointOnSlope(aabb.bottomLeft() + ivec2(0, 1), map, Tile::slope_down);
}

bool TerrainCollisionDetector::slopeUpIntersection(const AABB &aabb)
{
    return
        pointOnSlope(aabb.bottomRight() - ivec2(1, 0), map, Tile::slope_up)
        ||
        pointOnSlope(aabb.bottomRight() + ivec2(0, 1), map, Tile::slope_up);
}

bool TerrainCollisionDetector::slopedCeilingDownIntersection(const AABB &aabb)
{
    return
        pointOnSlope(aabb.topRight() - ivec2(1, 0), map, Tile::sloped_ceil_down)
        ||
        pointOnSlope(aabb.topRight() - ivec2(0, 1), map, Tile::sloped_ceil_down);
}

bool TerrainCollisionDetector::slopedCeilingUpIntersection(const AABB &aabb)
{
    return
        pointOnSlope(aabb.topLeft() + ivec2(1, 0), map, Tile::sloped_ceil_up)
        ||
        pointOnSlope(aabb.topLeft() - ivec2(0, 1), map, Tile::sloped_ceil_up);
}

bool TerrainCollisionDetector::floorIntersection(const AABB &aabb, bool ignorePlatforms)
{
    ivec2
        p0 = aabb.bottomLeft()  + ivec2(1, 0),
        p1 = aabb.bottomRight() - ivec2(1, 0);

    bool col = false;
    map->loopThroughTiles(p0, p1, [&](ivec2 t, Tile tile) {
        if (tile == Tile::full) col = true;
        else if (
                !ignorePlatforms && tile == Tile::platform
                && (p0.y % TileMap::PIXELS_PER_TILE) == TileMap::PIXELS_PER_TILE - 1
            )
            col = true;
    });
    return col;
}

bool TerrainCollisionDetector::leftWallIntersection(const AABB &aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.bottomLeft()  + ivec2(0, 1),
            aabb.topLeft()     - ivec2(0, 1),
            map
    );
}

bool TerrainCollisionDetector::rightWallIntersection(const AABB &aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.bottomRight() + ivec2(0, 1),
            aabb.topRight()    - ivec2(0, 1),
            map
    );
}
