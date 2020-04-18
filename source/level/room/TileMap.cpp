
#include <iostream>
#include <string.h>
#include <utils/gu_error.h>

#include "TileMap.h"
#include "../Level.h"

TileMap::TileMap(ivec2 size)
    :
    width(size.x), height(size.y),

    tiles(new Tile[size.x * size.y])
{
    std::fill_n(tiles, width * height, Tile::empty);
    std::cout << "TileMap (" << int(width) << "x" << int(height) << ") created\n";
}

void TileMap::fromBinary(const char *data, int size)
{
    if (width * height != size)
        throw gu_err("Trying to load a TileMap with invalid size");

    memcpy(tiles, &data[0], width * height);
    refreshOutlines();
    std::cout << "Loaded tileMap from binary.\n";
}

void TileMap::toBinary(std::vector<char> &out)
{
    int tilesOffset = out.size();

    out.resize(tilesOffset + width * height);

    memcpy(&out[tilesOffset], tiles, width * height);
}

TileMap::~TileMap()
{
    delete[] tiles;
}

Tile TileMap::getTile(uint8 x, uint8 y) const
{
    if (!contains(x, y)) return Tile::empty;
    return tiles[x * height + y];
}

void TileMap::setTile(uint8 x, uint8 y, Tile tile)
{
    if (contains(x, y))
    {
        tiles[x * height + y] = tile;

        tile_update update { x, y, uint8(tile) };
        tileUpdatesSinceLastUpdate.push_back(update);

        refreshOutlines();
    }
}

bool TileMap::contains(uint8 x, uint8 y) const
{
    return x < width && y < height;
}

void TileMap::refreshOutlines()
{
    outlines.clear();
    TileMapOutliner::getOutlines(this, outlines);
}

void TileMap::loopThroughTiles(const ivec2 &pixelCoordsMin, const ivec2 &pixelCoordsMax,
                            const std::function<void(ivec2 tileCoords, Tile tile)> &callback) const
{
    for (int x = pixelCoordsMin.x / TileMap::PIXELS_PER_TILE; x <= pixelCoordsMax.x / TileMap::PIXELS_PER_TILE; x++)
    {
        for (int y = pixelCoordsMin.y / TileMap::PIXELS_PER_TILE; y <= pixelCoordsMax.y / TileMap::PIXELS_PER_TILE; y++)
            callback(ivec2(x, y), getTile(x, y));
    }
}

