
#include "TerrainCollisionDetector.h"
#include "../../../generated/Physics.hpp"
#include "../../../generated/PolyPlatform.hpp"
#include "../../../generated/Polyline.hpp"
#include "../../components/component_methods.h"
#include "../../../level/room/TileMap.h"

void TerrainCollisionDetector::detect(
        TerrainCollisions &collisions, const AABB &aabb, bool ignorePlatforms, bool ignorePolyPlatforms, bool ignoreFluids
)
{
    collisions.polyPlatform = ignorePolyPlatforms ? false : onPolyPlatform(aabb, collisions, ignorePlatforms);
    collisions.fluid = ignoreFluids ? false : inFluid(aabb, collisions);

    collisions.halfSlopeDown = halfSlopeDownIntersection(aabb, collisions.floorMaterial);
    collisions.halfSlopeUp = halfSlopeUpIntersection(aabb, collisions.floorMaterial);
    collisions.slopeDown = collisions.halfSlopeDown || slopeDownIntersection(aabb, collisions.floorMaterial);
    collisions.slopeUp = collisions.halfSlopeUp || slopeUpIntersection(aabb, collisions.floorMaterial);
    collisions.flatFloor = floorIntersection(aabb, ignorePlatforms, collisions.floorMaterial);
    collisions.floor = collisions.polyPlatform || collisions.slopeUp || collisions.slopeDown || collisions.flatFloor;
    collisions.slopedCeilingDown = slopedCeilingDownIntersection(aabb);
    collisions.slopedCeilingUp = slopedCeilingUpIntersection(aabb);
    collisions.ceiling = collisions.slopedCeilingUp || collisions.slopedCeilingDown || ceilingIntersection(aabb);
    collisions.leftWall = collisions.slopedCeilingUp || leftWallIntersection(aabb);
    collisions.rightWall = collisions.slopedCeilingDown || rightWallIntersection(aabb);

    if (collisions.polyPlatform && collisions.ceiling)
    {
        // if space between platform and ceiling becomes too small -> prevent entity from walking even further.
        if (collisions.polyPlatformDeltaLeft > 0)
            collisions.leftWall = true;
        if (collisions.polyPlatformDeltaRight > 0)
            collisions.rightWall = true;
    }

    collisions.anything = collisions.floor || collisions.leftWall || collisions.rightWall || collisions.ceiling || collisions.fluid;
}

/**
 * Checks if the line from p0 to p1 intersects with Tile::full
 * @param p0    Should have smallest x and y
 * @param p1    Should have biggest x and y
 */
bool lineIntersectsWithFullBlock(const ivec2 &p0, const ivec2 &p1, const TileMap *map)
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
bool pointOnSlope(const ivec2 &p, const TileMap *map, Tile slopeType, TileMaterial &mat)
{
    int
        tileX = p.x / TileMap::PIXELS_PER_TILE,
        tileY = p.y / TileMap::PIXELS_PER_TILE;

    Tile tile = map->getTile(tileX, tileY);

    if (tile != slopeType)
        return false;

    int
        tilePixelX = p.x % TileMap::PIXELS_PER_TILE,
        tilePixelY = p.y % TileMap::PIXELS_PER_TILE;

    bool onSlope = false;

    switch (tile)
    {
        case Tile::slope_down:
            onSlope = tilePixelY <= TileMap::PIXELS_PER_TILE - 1 - tilePixelX;
            break;

        case Tile::slope_up:
            onSlope = tilePixelY <= tilePixelX;
            break;

        case Tile::sloped_ceil_down:
            onSlope = tilePixelY >= TileMap::PIXELS_PER_TILE - 1 - tilePixelX;
            break;

        case Tile::sloped_ceil_up:
            onSlope = tilePixelY >= tilePixelX;
            break;

        case Tile::slope_down_half0:
            onSlope = tilePixelY <= TileMap::PIXELS_PER_TILE - 1 - (tilePixelX * .5);
            break;

        case Tile::slope_down_half1:
            onSlope = tilePixelY <= TileMap::PIXELS_PER_TILE - 1 - (tilePixelX * .5) - 8;
            break;

        case Tile::slope_up_half0:
            onSlope = tilePixelY <= tilePixelX * .5;
            break;

        case Tile::slope_up_half1:
            onSlope = tilePixelY <= tilePixelX * .5 + 8;
            break;

        default: break;
    }
    if (onSlope)
        mat = map->getMaterial(tileX, tileY);
    return onSlope;
}

bool TerrainCollisionDetector::ceilingIntersection(const AABB &aabb)
{
    return lineIntersectsWithFullBlock(
            aabb.center + aabb.halfSize * ivec2(-1, 1) + ivec2(1, 0),
            aabb.center + aabb.halfSize                - ivec2(1, 0),
            map
    );
}

bool TerrainCollisionDetector::slopeDownIntersection(const AABB &aabb, TileMaterial &mat)
{
    return
        pointOnSlope(aabb.bottomLeft() + ivec2(1, 0), map, Tile::slope_down, mat)
        ||
        pointOnSlope(aabb.bottomLeft() + ivec2(0, 1), map, Tile::slope_down, mat);
}

bool TerrainCollisionDetector::slopeUpIntersection(const AABB &aabb, TileMaterial &mat)
{
    return
        pointOnSlope(aabb.bottomRight() - ivec2(1, 0), map, Tile::slope_up, mat)
        ||
        pointOnSlope(aabb.bottomRight() + ivec2(0, 1), map, Tile::slope_up, mat);
}

bool TerrainCollisionDetector::slopedCeilingDownIntersection(const AABB &aabb)
{
    static TileMaterial _;
    return
        pointOnSlope(aabb.topRight() - ivec2(1, 0), map, Tile::sloped_ceil_down, _)
        ||
        pointOnSlope(aabb.topRight() - ivec2(0, 1), map, Tile::sloped_ceil_down, _);
}

bool TerrainCollisionDetector::slopedCeilingUpIntersection(const AABB &aabb)
{
    static TileMaterial _;
    return
        pointOnSlope(aabb.topLeft() + ivec2(1, 0), map, Tile::sloped_ceil_up, _)
        ||
        pointOnSlope(aabb.topLeft() - ivec2(0, 1), map, Tile::sloped_ceil_up, _);
}

bool TerrainCollisionDetector::floorIntersection(const AABB &aabb, bool ignorePlatforms, TileMaterial &mat)
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
        if (col)
            mat = map->getMaterial(t.x, t.y);
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

bool TerrainCollisionDetector::halfSlopeDownIntersection(const AABB &aabb, TileMaterial &mat)
{
    return
            pointOnSlope(aabb.bottomLeft() + ivec2(1, 0), map, Tile::slope_down_half0, mat)
            ||
            pointOnSlope(aabb.bottomLeft() + ivec2(0, 1), map, Tile::slope_down_half0, mat)
            ||
            pointOnSlope(aabb.bottomLeft() + ivec2(1, 0), map, Tile::slope_down_half1, mat)
            ||
            pointOnSlope(aabb.bottomLeft() + ivec2(0, 1), map, Tile::slope_down_half1, mat);
}

bool TerrainCollisionDetector::halfSlopeUpIntersection(const AABB &aabb, TileMaterial &mat)
{
    return
            pointOnSlope(aabb.bottomRight() - ivec2(1, 0), map, Tile::slope_up_half0, mat)
            ||
            pointOnSlope(aabb.bottomRight() + ivec2(0, 1), map, Tile::slope_up_half0, mat)
            ||
            pointOnSlope(aabb.bottomRight() - ivec2(1, 0), map, Tile::slope_up_half1, mat)
            ||
            pointOnSlope(aabb.bottomRight() + ivec2(0, 1), map, Tile::slope_up_half1, mat);
}

bool TerrainCollisionDetector::onPolyPlatform(const AABB &aabb, TerrainCollisions &col, bool fallThrough)
{
    if (!reg) return false;
    col.abovePolyPlatformEntity = entt::null;
    col.pixelsAbovePolyPlatform = 0;

    ivec2 point = aabb.bottomCenter();
    int xLeft = point.x - 1, xRight = point.x + 1;

    auto test = [&](
            entt::entity e, AABB &platformAABB, PolyPlatform &platform, Polyline &line
    ) -> bool {

        if ((fallThrough && platform.allowFallThrough) || !platformAABB.contains(point))
            return false;

        int nrOfPoints = line.points.size();

        if (nrOfPoints < 2)
            return false; // very big infinite loop if you dont return here, woops

        auto it = line.points.begin();

        int heightLeft = -99, height = 0, heightRight = 0;

        for (int i = 0; i < nrOfPoints - 1; i++)
        {
            const vec2 p0 = *it + vec2(platformAABB.center);
            const vec2 p1 = *(++it) + vec2(platformAABB.center);

            int xMin = p0.x, xMax = p1.x - 1;
            if (xMax >= xLeft && xMin <= xLeft)
                heightLeft = line.heightAtX(xLeft, p0, p1);
            if (xMax >= point.x && xMin <= point.x)
                height = line.heightAtX(point.x, p0, p1);
            if (xMax >= xRight && xMin <= xRight)
            {
                if (height == point.y) // entity is ON the platform:
                {
                    if (heightLeft == -99)
                        heightLeft = height;

                    heightRight = line.heightAtX(xRight, p0, p1);

                    col.polyPlatformDeltaLeft = heightLeft - height;
                    col.polyPlatformDeltaRight = heightRight - height;
                    col.floorMaterial = platform.material;
                    return true;
                }
                else if (height < point.y) // entity is above platform:
                {
                    uint8 diff = point.y - height;
                    if (diff <= DETECT_POLY_PLATFORM_MARGIN && (col.abovePolyPlatformEntity != entt::null || col.pixelsAbovePolyPlatform < diff))
                    {
                        col.abovePolyPlatformEntity = e;
                        col.pixelsAbovePolyPlatform = diff;
                    }
                }
            }
        }
        return false;
    };

    // first check if still on previous platform:
    if (col.polyPlatformEntity != entt::null)
    {
        AABB *platformAABB = reg->try_get<AABB>(col.polyPlatformEntity);
        PolyPlatform *platform = reg->try_get<PolyPlatform>(col.polyPlatformEntity);
        Polyline *line = reg->try_get<Polyline>(col.polyPlatformEntity);

        if (platformAABB && platform && line && test(col.polyPlatformEntity, *platformAABB, *platform, *line))
            return true;
    }

    bool foundPlatform = false;

    // check all other platforms:
    reg->view<AABB, PolyPlatform, Polyline>()
    .each([&](auto e, AABB &platformAABB, PolyPlatform &platform, Polyline &line){

        if (foundPlatform || e == col.polyPlatformEntity)
            return;

        if (test(e, platformAABB, platform, line))
        {
            foundPlatform = true;
            col.polyPlatformEntity = e;
        }
    });
    if (foundPlatform)
        return true;

    col.polyPlatformEntity = entt::null;
    col.polyPlatformDeltaLeft = col.polyPlatformDeltaRight = 0;
    return false;
}

bool TerrainCollisionDetector::inFluid(const AABB &aabb, TerrainCollisions &col)
{
    bool foundFluid = false;
    col.fluidDepth = 255;

    auto test = [&](auto fluidEntity, const AABB &fluidBox, Fluid &fluid) {

        if (foundFluid || !overlap(fluidBox, aabb))
            return;

        Polyline *surfaceLine = reg->try_get<Polyline>(fluidEntity);
        if (surfaceLine)
        {
            int pointIndex = 0;
            float height = surfaceLine->heightAtX(aabb.center.x, fluidBox.center, pointIndex);
            int depth = height - (aabb.center.y - aabb.halfSize.y);
            if (depth < 0)
                return;
            col.fluidDepth = depth;
            col.fluidSurfaceLineXIndex = pointIndex;
        }

        col.fluidEntity = fluidEntity;
        foundFluid = true;
    };

    if (col.fluidEntity != entt::null) // entity was in a fluid previous frame, test if entity is still in the same fluid before looping over all other fluids.
    {
        const AABB *aabb = reg->try_get<AABB>(col.fluidEntity);
        Fluid *fluid = reg->try_get<Fluid>(col.fluidEntity);
        if (aabb && fluid)
            test(col.fluidEntity, *aabb, *fluid);
    }

    if (!foundFluid)
    {
        col.fluidEntity = entt::null;
        reg->view<AABB, Fluid>().each(test);
    }

    return foundFluid;
}
