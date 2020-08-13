
#ifndef GAME_MEGASPRITESHEET_H
#define GAME_MEGASPRITESHEET_H

#include <utils/aseprite/Aseprite.h>
#include <graphics/texture.h>
#include <map>

/**
 * a huge texture containing all sprites of the game
 */
class MegaSpriteSheet
{
  public:

    struct SubSheet
    {
        std::vector<ivec2> frameOffsets;
    };

    SharedTexture texture;

    static constexpr int SIZE = 1024;

    void add(const aseprite::Sprite &);

    const SubSheet &spriteInfo(const aseprite::Sprite &) const;

    void printUsage() const;

  private:

    const static int CHUNK_SIZE = 8, CHUNKS_PER_ROW = SIZE / CHUNK_SIZE;

    bool used[CHUNKS_PER_ROW][CHUNKS_PER_ROW] = {};

    std::map<const aseprite::Sprite *, SubSheet> subSheets;

    bool tryReserve(const ivec2 &chunkOffset, const ivec2 &chunkSize);

    void createTexture();

};


#endif
