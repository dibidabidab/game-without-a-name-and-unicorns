
#ifndef GAME_ROOM_H
#define GAME_ROOM_H

#include <vector>
#include <utils/math_utils.h>

enum class Tile : unsigned char
{
    /*                                      ___               ___
     *     ___      |\           /|         \  |             |  /
     *    |   |     | \         / |          \ |             | /
     *    |___|     |__\       /__|           \|             |/
     */
    empty, full, slope_down, slope_up, sloped_ceil_down, sloped_ceil_up
};

class Room
{

    Tile *tiles;

  public:

    constexpr const static auto TILE_TYPES = { Tile::empty, Tile::full, Tile::slope_down, Tile::slope_up, Tile::sloped_ceil_down, Tile::sloped_ceil_up };

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

    inline Tile getTile(uint8 x, uint8 y)
    {
        if (!inRoom(x, y)) return Tile::empty;
        return tiles[x * height + y];
    }

    inline void setTile(uint8 x, uint8 y, Tile tile)
    {
        if (inRoom(x, y)) tiles[x * height + y] = tile;
    }

    inline bool inRoom(uint8 x, uint8 y)
    {
        return x < width && y < height;
    }

    /**
     * Exports this room to binary data, which will be APPENDED to `out`
     */
    void toBinary(std::vector<unsigned char> &out);

};


#endif
