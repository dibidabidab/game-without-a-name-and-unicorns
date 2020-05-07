
#ifndef GAME_MEGASPRITESHEET_H
#define GAME_MEGASPRITESHEET_H

#include <utils/aseprite/Aseprite.h>
#include <graphics/texture.h>
#include <map>

class MegaSpriteSheet
{
  public:

    struct SubSheet
    {
        std::vector<ivec2> frameOffsets;
    };

    SharedTexture texture;

    const static int SIZE = 1024;

    void add(const aseprite::Sprite &);

    const SubSheet &spriteInfo(const aseprite::Sprite &) const;

  private:

    const static int CHUNK_SIZE = 16, CHUNKS_PER_ROW = SIZE / CHUNK_SIZE;

    bool used[CHUNKS_PER_ROW][CHUNKS_PER_ROW] = {};

    std::map<const aseprite::Sprite *, SubSheet> subSheets;

    bool tryReserve(const ivec2 &chunkOffset, const ivec2 &chunkSize);

    void createTexture();

};


#endif
