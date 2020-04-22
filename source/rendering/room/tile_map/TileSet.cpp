
#include "TileSet.h"
#include <utils/aseprite/AsepriteLoader.h>
#include <utils/aseprite/AsepriteTextureGenerator.h>
#include <utils/string.h>

TileSet::TileSet(const char *filePath)
{
    std::cout << "Loading tileset " << filePath << '\n';
    aseprite::Loader(filePath, sprite);

    for (int i = 0; i < sprite.frameCount; i++)
        variations.push_back(aseprite::TextureGenerator::frameToTexture(sprite, i));

    for (auto &slice : sprite.slices)
        subTextures.emplace_back(slice);
}

const TileSet::SubTexture *TileSet::getSubTextureForTile(const TileMap &map, int x, int y)
{
    SubTexture *best = NULL;
    int bestScore = -1;

    for (int i = 0; i < subTextures.size(); i++)
    {
        SubTexture *subT = &subTextures[i];
        int score = subT->match(map, x, y);
        if (score > bestScore)
        {
            best = subT;
            bestScore = score;
        }
    }
    return best;
}

int TileSet::SubTexture::match(const TileMap &map, int tx, int ty)
{
    int result = 0;
    int i = 0;
    for (int y = ty + 1; y > ty - 2; y--)
    {
        for (int x = tx - 1; x < tx + 2; x++)
        {
            Tile tile = map.getTile(x, y);
            std::string n = pattern.at(i++);

            if (n == "*")
                result += 1;
            else if (n == "a")
            {
                if (tile == Tile::empty)
                    return -1;
                result += 10;
            }
            else
            {
                Tile nt = Tile(atoi(n.c_str()));
                if (nt != tile)
                    return -1;
                result += 100;
            }
        }
    }
    return result;
}

TileSet::SubTexture::SubTexture(aseprite::Slice &slice)
{
    offset = ivec2(slice.originX, slice.originY);

    if (slice.width != 48 || slice.height != 48)
        throw gu_err("TileSet has slice that is not 48x48 pixels");

    auto rows = splitString(slice.name, " ");
    if (rows.size() != 3)
        throw gu_err("TileSet has slice that has invalid name/pattern");
    for (int i = 0; i < 3; i++)
    {
        auto cols = splitString(rows[i], ",");
        if (cols.size() != 3)
            throw gu_err("TileSet has slice that has invalid name/pattern");
        pattern.insert(pattern.end(), cols.begin(), cols.end());
    }
}
