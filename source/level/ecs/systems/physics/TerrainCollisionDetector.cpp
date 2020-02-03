
#include "TerrainCollisionDetector.h"
#include "../../components/Physics.h"
#include "../../../Level.h"

TerrainCollisions TerrainCollisionDetector::detect(AABB aabb)
{
    TerrainCollisions collisions;

    collisions.ceiling = ceilingIntersection(aabb);
    collisions.leftWall = leftWallIntersection(aabb);
    collisions.rightWall = rightWallIntersection(aabb);
    collisions.slopeDown = slopeDownIntersection(aabb);
    collisions.slopeUp = slopeUpIntersection(aabb);
    collisions.floor = collisions.slopeUp || collisions.slopeDown || floorIntersection(aabb);

    return collisions;
}

bool lineIntersectsWithFullBlock(ivec2 p0, ivec2 p1, Room *room)
{
    bool col = false;
    room->loopThroughTiles(p0, p1, [&](ivec2 t, Tile tile) {
       if (tile == Tile::full) col = true;
    });
    return col;
}

bool TerrainCollisionDetector::ceilingIntersection(AABB aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.center + aabb.halfSize * ivec2(-1, 1) + ivec2(1, 0),
            aabb.center + aabb.halfSize                - ivec2(1, 0),
            room
    );
}

bool TerrainCollisionDetector::slopeDownIntersection(AABB aabb)
{
//    ivec2 corner = aabb.center - aabb.halfSize;
//    int
//            tileX = corner.x / Level::PIXELS_PER_BLOCK,
//            tileY = corner.y / Level::PIXELS_PER_BLOCK;
//
//    if (room->getTile(tileX, tileY) == Tile::slope_up)
//    {
//        int
//                tilePixelX = corner.x % Level::PIXELS_PER_BLOCK,
//                tilePixelY = corner.y % Level::PIXELS_PER_BLOCK;
//        return tilePixelY > tilePixelX;
//    }
    return false;
}

bool TerrainCollisionDetector::slopeUpIntersection(AABB aabb)
{
    ivec2 corner = aabb.center - aabb.halfSize;
    int
        tileX = corner.x / Level::PIXELS_PER_BLOCK,
        tileY = corner.y / Level::PIXELS_PER_BLOCK;

    if (room->getTile(tileX, tileY) == Tile::slope_up)
    {
        int
            tilePixelX = corner.x % Level::PIXELS_PER_BLOCK,
            tilePixelY = corner.y % Level::PIXELS_PER_BLOCK;
        return tilePixelY > tilePixelX;
    }
    return false;
}

bool TerrainCollisionDetector::floorIntersection(AABB aabb)
{
    ivec2
        p0 = aabb.center - aabb.halfSize                + ivec2(1, 0),
        p1 = aabb.center + aabb.halfSize * ivec2(1, -1) - ivec2(1, 0);

    bool col = false;
    room->loopThroughTiles(p0, p1, [&](ivec2 t, Tile tile) {
        if (tile == Tile::full) col = true;
        else if (tile == Tile::platform && (p0.y % Level::PIXELS_PER_BLOCK) == Level::PIXELS_PER_BLOCK - 1) col = true;
    });
    return col;
}

bool TerrainCollisionDetector::leftWallIntersection(AABB aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.center - aabb.halfSize                + ivec2(0, 1),
            aabb.center + aabb.halfSize * ivec2(-1, 1) - ivec2(0, 1),
            room
    );
}

bool TerrainCollisionDetector::rightWallIntersection(AABB aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.center - aabb.halfSize * ivec2(-1, 1) + ivec2(0, 1),
            aabb.center + aabb.halfSize                - ivec2(0, 1),
            room
    );
}
