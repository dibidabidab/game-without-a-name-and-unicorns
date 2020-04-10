
#ifndef GAME_mapOUTLINER_H
#define GAME_mapOUTLINER_H

#include <vector>
#include <utils/math_utils.h>

class TileMap;

namespace TileMapOutliner
{
    /**
     * Finds all outlines of the map based on it's tiles.
     *
     * @param map  The map
     * @param out   All found outlines are added to this vector
     */
    void getOutlines(const TileMap *map, std::vector<std::pair<ivec2, ivec2>> &out);

    void getWalls(const TileMap *map, std::vector<std::pair<ivec2, ivec2>> &out);

    void getFloorsAndCeilings(const TileMap *map, std::vector<std::pair<ivec2, ivec2>> &out);

    void getUpwardSlopes(const TileMap *map, std::vector<std::pair<ivec2, ivec2>> &out);

    void getDownwardSlopes(const TileMap *map, std::vector<std::pair<ivec2, ivec2>> &out);
};


#endif
