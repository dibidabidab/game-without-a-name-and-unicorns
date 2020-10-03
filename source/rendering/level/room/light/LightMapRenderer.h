
#ifndef GAME_LIGHTMAPRENDERER_H
#define GAME_LIGHTMAPRENDERER_H


#include <graphics/frame_buffer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include <graphics/3d/mesh.h>
#include <graphics/3d/vert_buffer.h>
#include "../../../../tiled_room/TiledRoom.h"

class LightMapRenderer
{
    TiledRoom *room;

    ShaderAsset pointLightShader, directionalLightShader;

    SharedMesh quadMesh;
    VertData pointLightsData, directionalLightsData;
    int pointLightsDataBuffer = -1;
    int directionalLightsDataBuffer = -1;

  public:

    FrameBuffer *fbo = NULL;

    LightMapRenderer(TiledRoom *room);

    void render(const Camera &cam, const SharedTexture &shadowTexture);

    ~LightMapRenderer();

  private:

    void renderPointLights(const Camera &cam, const SharedTexture &shadowTexture);

    void renderDirectionalLights(const Camera &cam);

};


#endif
