
#ifndef GAME_TILEMAPRENDERER_H
#define GAME_TILEMAPRENDERER_H


#include <utils/aseprite/Aseprite.h>
#include <asset_manager/asset.h>
#include <graphics/frame_buffer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include "../../../../tiled_room/TileMap.h"
#include "TileSet.h"

class TileMapRenderer
{

    TileMap *map;

    ShaderAsset tileShader, mapShader;

    bool textureCreated = false;

    double lastTileSetReloadTime = 0;

  public:

    TileMapRenderer(TileMap *);

    void updateMapTextureIfNeeded();

    void render(const Camera &cam, const SharedTexture &bloodSplatterTexture);

    FrameBuffer fbo;
  private:
    void createMapTexture();

    void renderTile(int x, int y);
};


#endif
