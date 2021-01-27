
#ifndef GAME_FIRERENDERER_H
#define GAME_FIRERENDERER_H

#include <graphics/shader_asset.h>
#include <graphics/camera.h>
#include <graphics/frame_buffer.h>
#include <graphics/3d/mesh.h>
#include "../../../../external/entt/src/entt/entity/registry.hpp"

class FireRenderer
{

    ShaderAsset particleShader, combineShader;

    SharedMesh quad;
    VertData particleData;
    int particleDataID = -1;

  public:

    FrameBuffer *fbo = NULL;

    FireRenderer();

    void renderParticles(entt::registry &, const Camera &);

    void renderCombined(float time, const Camera &);

    void onResize(const Camera &);
};


#endif
