
#include <iostream>
#include <string.h>
#include <utils/gu_error.h>

#include "TileMap.h"

TileMap::TileMap(ivec2 size)
    :
    width(size.x), height(size.y),

    tiles(size.x * size.y, Tile::empty),
    tileMaterials(size.x * size.y, 0),
    materialProperties(asset<json>("tile_materials").get().get<decltype(materialProperties)>()),
    nrOfMaterialTypes(materialProperties.size())
{
}

void TileMap::fromBinary(const char *data, int size)
{
    if (width * height * 2 != size)
        throw gu_err("Trying to load a TileMap with invalid size");

    memcpy(tiles.data(), &data[0], width * height);
    memcpy(tileMaterials.data(), &data[width * height], width * height);
    refreshOutlines();
}

void TileMap::toBinary(std::vector<char> &out) const
{
    int tilesOffset = out.size();

    out.resize(tilesOffset + width * height * 2);

    memcpy(&out[tilesOffset], tiles.data(), width * height);
    memcpy(&out[tilesOffset + (width * height)], tileMaterials.data(), width * height);
}

Tile TileMap::getTile(uint8 x, uint8 y) const
{
    if (!contains(x, y)) return Tile::full;
    return tiles[x * height + y];
}

TileMaterial TileMap::getMaterial(uint8 x, uint8 y) const
{
    if (!contains(x, y)) return 0;
    return tileMaterials[x * height + y];
}

void TileMap::setTile(uint8 x, uint8 y, Tile tile)
{
    setTile(x, y, tile, getMaterial(x, y));
}

void TileMap::setTile(uint8 x, uint8 y, Tile tile, TileMaterial material, bool registerAsUpdate, bool refreshOutlines)
{
    if (!contains(x, y))
        return;

    auto &t = tiles[x * height + y];
    auto &m = tileMaterials[x * height + y];

    if (t == tile && m == material)
        return; // current tile+material is the same. Prevent rerender of tileMap and sending updates over network..

    t = tile;
    m = material;

    if (registerAsUpdate)
    {
        tile_update update;
        update.x = x;
        update.y = y;
        update.newTile = uint8(tile);
        update.newTileMaterial = material;
        tileUpdatesSinceLastUpdate.push_back(update);
    }

    if (refreshOutlines)
        this->refreshOutlines();
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

const TileMaterialProperties &TileMap::getMaterialProperties(TileMaterial m) const
{
    if (m >= materialProperties.size())
        throw gu_err("No TileMaterialProperties found for TileMaterial#" + std::to_string(int(m)));
    return materialProperties[m];
}

void TileMap::copy(const TileMap &other, int fromX, int fromY, int toX, int toY, int width, int height)
{
    for (int w = 0; w < width; w++)
        for (int h = 0; h < height; h++)
            setTile(toX + w, toY + h, other.getTile(fromX + w, fromY + h), other.getMaterial(fromX + w, fromY + h));
}
