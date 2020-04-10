
#ifndef GAME_TILEMAP_H
#define GAME_TILEMAP_H

#include <vector>
#include <functional>
#include <utils/math_utils.h>
#include "TileMapOutliner.h"
#include "../ecs/systems/EntitySystem.h"

enum class Tile : unsigned char
{
    /*                                      ___               ___        _______
     *     ___      |\           /|         \  |             |  /
     *    |   |     | \         / |          \ |             | /
     *    |___|     |__\       /__|           \|             |/
     */
    empty, full, slope_down, slope_up, sloped_ceil_down, sloped_ceil_up, platform
};

typedef std::vector<std::pair<ivec2, ivec2>> TileMapOutlines;

class TileMap
{

    Tile *tiles;
    TileMapOutlines outlines;

  public:

    static const int PIXELS_PER_TILE = 16;

    constexpr const static auto TILE_TYPES = { Tile::empty, Tile::full, Tile::slope_down, Tile::slope_up, Tile::sloped_ceil_down, Tile::sloped_ceil_up, Tile::platform };

    const uint8 width, height;

    /**
     * Creates an empty map
     */
    TileMap(ivec2 size);

    ~TileMap();

    Tile getTile(uint8 x, uint8 y) const;

    void setTile(uint8 x, uint8 y, Tile tile);

    bool contains(uint8 x, uint8 y) const;

    const TileMapOutlines &getOutlines() const { return outlines; }

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


  private:
    void refreshOutlines();

};


#endif
