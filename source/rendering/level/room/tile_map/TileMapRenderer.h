
#ifndef GAME_TILEMAPRENDERER_H
#define GAME_TILEMAPRENDERER_H


#include <utils/aseprite/Aseprite.h>
#include <asset_manager/asset.h>
#include <graphics/frame_buffer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>

class TiledRoom;
class TileMap;

class TileMapRenderer
{

    const TiledRoom &room;

    ShaderAsset tileShader, mapShader;

    bool textureCreated = false;

    std::vector<float> prevZIndexes;
    double lastTileSetReloadTime = 0;

  public:

    TileMapRenderer(const TiledRoom &);

    void updateMapTextureIfNeeded();

    void render(const Camera &, const SharedTexture &bloodSplatterTexture);

    FrameBuffer fbo;
  private:
    void createMapTexture();

    void renderTile(const TileMap &, int x, int y);
};


#endif
