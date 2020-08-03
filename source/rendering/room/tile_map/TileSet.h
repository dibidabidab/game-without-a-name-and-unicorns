
#ifndef GAME_TILESET_H
#define GAME_TILESET_H


#include <utils/aseprite/Aseprite.h>
#include <graphics/texture.h>

class TileMap;

class TileSet
{

    struct SubTexture
    {
        SubTexture(aseprite::Slice &);

        ivec2 offset;

        std::vector<std::string> pattern;

        int match(const TileMap &, int x, int y) const;
    };

    aseprite::Sprite sprite;
    std::vector<SubTexture> subTextures;

  public:
    TileSet(const char *filePath);

    const SubTexture *getSubTextureForTile(const TileMap &, int x, int y) const;

    std::vector<SharedTexture> variations;
};


#endif
