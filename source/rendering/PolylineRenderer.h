
#ifndef GAME_POLYLINERENDERER_H
#define GAME_POLYLINERENDERER_H


#include <graphics/3d/mesh.h>
#include <graphics/shader_asset.h>
#include <graphics/camera.h>
#include "../../entt/src/entt/entity/registry.hpp"

class PolylineRenderer
{

    ShaderAsset shader;

    SharedMesh lineSegmentMesh;
    VertData lineSegments;
    int lineSegmentsID = -1;

  public:

    PolylineRenderer();

    void render(const entt::registry &, const Camera &);

};


#endif
