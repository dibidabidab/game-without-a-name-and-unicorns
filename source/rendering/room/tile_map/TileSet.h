
#ifndef GAME_TILESET_H
#define GAME_TILESET_H


#include <utils/aseprite/Aseprite.h>
#include <graphics/texture.h>
#include "../../../level/room/TileMap.h"

class TileSet
{

    struct SubTexture
    {
        SubTexture(aseprite::Slice &);

        ivec2 offset;

        std::vector<std::string> pattern;

        int match(const TileMap &, int x, int y);
    };

    aseprite::Sprite sprite;
    std::vector<SubTexture> subTextures;

  public:
    TileSet(const char *filePath);

    const SubTexture *getSubTextureForTile(const TileMap &, int x, int y);

    std::vector<SharedTexture> variations;
};


#endif
