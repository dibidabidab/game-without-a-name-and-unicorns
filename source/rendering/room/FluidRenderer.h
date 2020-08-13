
#ifndef GAME_FLUIDRENDERER_H
#define GAME_FLUIDRENDERER_H

#include <graphics/3d/mesh.h>
#include <graphics/shader_asset.h>
#include <graphics/camera.h>
#include <graphics/frame_buffer.h>
#include "../../external/entt/src/entt/entity/registry.hpp"

class FluidRenderer
{

    ShaderAsset shader, reflectionsShader;

    SharedMesh trapezoidMesh;
    VertData segments;
    int segmentsID = -1;

    int nrOfSegments = 0;

  public:

    FrameBuffer *reflectionsFbo = NULL;

    FluidRenderer();

    void render(entt::registry &, const Camera &);

    void renderReflections(FrameBuffer *indexedImage, const Camera &);

    void onResize(const Camera &);
};


#endif
