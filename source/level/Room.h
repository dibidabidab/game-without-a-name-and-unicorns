
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <vector>
#include <functional>
#include <utils/math_utils.h>
#include "RoomOutliner.h"

enum class Tile : unsigned char
{
    /*                                      ___               ___        _______
     *     ___      |\           /|         \  |             |  /
     *    |   |     | \         / |          \ |             | /
     *    |___|     |__\       /__|           \|             |/
     */
    empty, full, slope_down, slope_up, sloped_ceil_down, sloped_ceil_up, platform
};

typedef std::vector<std::pair<ivec2, ivec2>> RoomOutlines;

class Room
{

    Tile *tiles;
    RoomOutlines outlines;

  public:

    constexpr const static auto TILE_TYPES = { Tile::empty, Tile::full, Tile::slope_down, Tile::slope_up, Tile::sloped_ceil_down, Tile::sloped_ceil_up, Tile::platform };

    const uint8 width, height;

    /**
     * Creates an empty room
     */
    Room(uint8 width, uint8 height);

    /**
     * Loads a room from binary data.
     * Kinda really unsafe, if the data lies about it's width and height then wrong memory will be copied.
     */
    explicit Room(uint8 *data);

    ~Room();

    Tile getTile(uint8 x, uint8 y) const;

    void setTile(uint8 x, uint8 y, Tile tile);

    bool inRoom(uint8 x, uint8 y) const;

    const RoomOutlines &getOutlines() const { return outlines; }

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
     * Exports this room to binary data, which will be APPENDED to `out`
     */
    void toBinary(std::vector<unsigned char> &out);

  private:
    void refreshOutlines();

};


#endif
