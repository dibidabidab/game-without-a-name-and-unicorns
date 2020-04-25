
#ifndef GAME_SHADOWCASTER_H
#define GAME_SHADOWCASTER_H


#include <graphics/frame_buffer.h>
#include "../../../level/room/TileMap.h"

class ShadowCaster
{
    FrameBuffer fbo;

    const static int TEXTURE_SIZE = 2048, SIZE_PER_LIGHT = 256;

  public:

    ShadowCaster();

    std::vector<std::pair<vec2, vec2>> shadowCasters;

    void tileMapUpdated(const TileMap *map);

};


#endif
