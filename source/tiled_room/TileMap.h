
#ifndef GAME_TILEMAP_H
#define GAME_TILEMAP_H

#include <vector>
#include <list>
#include <functional>
#include <utils/math_utils.h>
#include "TileMapOutliner.h"
#include <ecs/systems/EntitySystem.h>
#include "../rendering/level/room/tile_map/TileSet.h"
#include "WindMap.h"

#include "../generated/TileMap.hpp"

enum class Tile : unsigned char {
    /*                                      ___               ___        _______
     *     ___      |\           /|         \  |             |  /
     *    |   |     | \         / |          \ |             | /
     *    |___|     |__\       /__|           \|             |/
     */
    empty, full, slope_down, slope_up, sloped_ceil_down, sloped_ceil_up, platform,

    // same as slope_down, but spread over 2 tiles:
    slope_down_half0, slope_down_half1,

    // same as slope_up, but spread over 2 tiles:
    slope_up_half0, slope_up_half1,
};

typedef std::vector<std::pair<vec2, vec2>> TileMapOutlines;
typedef uint8 TileMaterial;

class TileMap {

    Tile *tiles;
    TileMaterial *tileMaterials;

    TileMapOutlines outlines;

    std::list<tile_update> tileUpdatesSinceLastUpdate, tileUpdatesPrevUpdate;

    friend class TiledRoom;

    const std::vector<TileMaterialProperties> materialProperties;

public:

    static const int PIXELS_PER_TILE = 16;

    constexpr const static auto TILE_TYPES = {
            Tile::empty, Tile::full,
            Tile::slope_down, Tile::slope_up, Tile::sloped_ceil_down, Tile::sloped_ceil_up,
            Tile::platform,
            Tile::slope_down_half0, Tile::slope_down_half1,
            Tile::slope_up_half0, Tile::slope_up_half1
    };

    const uint8 width, height, nrOfMaterialTypes;

    WindMap wind;

    /**
     * Creates an empty map
     */
    TileMap(ivec2 size);

    ~TileMap();

    Tile getTile(uint8 x, uint8 y) const;

    TileMaterial getMaterial(uint8 x, uint8 y) const;

    const TileMaterialProperties &getMaterialProperties(TileMaterial) const;

    void setTile(uint8 x, uint8 y, Tile);

    void setTile(uint8 x, uint8 y, Tile, TileMaterial, bool registerAsUpdate = true, bool refreshOutlines = true);

    bool contains(uint8 x, uint8 y) const;

    void copy(const TileMap &other, int fromX, int fromY, int toX, int toY, int width, int height);

    const TileMapOutlines &getOutlines() const { return outlines; }

    const std::list<tile_update> &updatesSinceLastUpdate() const { return tileUpdatesSinceLastUpdate; }

    const std::list<tile_update> &updatesPrevUpdate() const { return tileUpdatesPrevUpdate; }

    /**
     * Loops through the room's tiles in a 2 dimensional loop
     * @param pixelCoordsMin    position of the first tile in pixels
     * @param pixelCoordsMax    position of the last tile in pixels
     * @param callback          function to be called for each tile
     */
    void loopThroughTiles(
            const ivec2 &pixelCoordsMin, const ivec2 &pixelCoordsMax,

            const std::function<void(ivec2 tileCoords, Tile tile)> &callback
    ) const;

    /**
     * Exports this map to binary data, which will be APPENDED to `out`
     */
    void toBinary(std::vector<char> &out);

    /**
     * Loads a map from binary data.
     */
    void fromBinary(const char *data, int size);

    void refreshOutlines();

};


#endif
