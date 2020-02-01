
#ifndef GAME_ROOMOUTLINER_H
#define GAME_ROOMOUTLINER_H

#include <vector>
#include <utils/math_utils.h>

class Room;

namespace RoomOutliner
{
    /**
     * Finds all outlines of the room based on it's tiles.
     *
     * @param room  The room
     * @param out   All found outlines are added to this vector
     */
    void getOutlines(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out);

    void getWalls(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out);

    void getFloorsAndCeilings(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out);

    void getUpwardSlopes(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out);

    void getDownwardSlopes(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out);
};


#endif
