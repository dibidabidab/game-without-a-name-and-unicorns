
#ifndef GAME_TILEMAPRENDERER_H
#define GAME_TILEMAPRENDERER_H


#include <utils/aseprite/Aseprite.h>
#include <asset_manager/asset.h>
#include <graphics/frame_buffer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include "../../../level/room/TileMap.h"
#include "TileSet.h"

class TileMapRenderer
{

    TileMap *map;

    ShaderAsset tileShader, mapShader;

    bool textureCreated = false;

  public:

    std::map<TileMaterial, asset<TileSet>> tileSets;

    TileMapRenderer(TileMap *);

    void updateMapTextureIfNeeded();

    void render(Camera &cam);

    FrameBuffer fbo;
  private:
    void createMapTexture();

    void renderTile(int x, int y);
};


#endif
