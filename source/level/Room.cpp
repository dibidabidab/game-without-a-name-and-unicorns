
#include <iostream>
#include <string.h>

#include "Room.h"
#include "Level.h"

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

Tile Room::getTile(uint8 x, uint8 y) const
{
    if (!inRoom(x, y)) return Tile::empty;
    return tiles[x * height + y];
}

void Room::setTile(uint8 x, uint8 y, Tile tile)
{
    if (inRoom(x, y))
    {
        tiles[x * height + y] = tile;
        refreshOutlines();
    }
}

bool Room::inRoom(uint8 x, uint8 y) const
{
    return x < width && y < height;
}

void Room::refreshOutlines()
{
    outlines.clear();
    RoomOutliner::getOutlines(this, outlines);
}

void Room::loopThroughTiles(const ivec2 &pixelCoordsMin, const ivec2 &pixelCoordsMax,
                            const std::function<void(ivec2 tileCoords, Tile tile)> &callback) const
{
    for (int x = pixelCoordsMin.x / Level::PIXELS_PER_BLOCK; x <= pixelCoordsMax.x / Level::PIXELS_PER_BLOCK; x++)
    {
        for (int y = pixelCoordsMin.y / Level::PIXELS_PER_BLOCK; y <= pixelCoordsMax.y / Level::PIXELS_PER_BLOCK; y++)
            callback(ivec2(x, y), getTile(x, y));
    }
}

