
#include <iostream>
#include <string.h>

#include "Room.h"

Room::Room(uint8 width, uint8 height)
    :
    width(width), height(height),

    tiles(new Tile[width * height])
{
    std::fill_n(tiles, width * height, Tile::empty);
    std::cout << "Room (" << int(width) << "x" << int(height) << ") created\n";
}

void Room::toBinary(std::vector<unsigned char> &out)
{
    out.push_back(width);
    out.push_back(height);

    uint8 tilesOffset = out.size();

    out.resize(tilesOffset + width * height);

    memcpy(&out[tilesOffset], tiles, width * height);
}

Room::Room(uint8 *data) : Room(data[0], data[1])
{
    memcpy(tiles, &data[2], width * height);
    std::cout << "Loaded room from binary.\n";
    refreshOutlines();
}

Room::~Room()
{
    delete[] tiles;
}
