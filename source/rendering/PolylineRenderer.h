
#ifndef GAME_POLYLINERENDERER_H
#define GAME_POLYLINERENDERER_H


#include <graphics/3d/mesh.h>
#include <graphics/shader_asset.h>
#include <graphics/camera.h>
#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../level/room/ecs/components/graphics/DrawPolyline.h"

class PolylineRenderer
{

    ShaderAsset shader;

    SharedMesh lineSegmentMesh;
    VertData lineSegments;
    int lineSegmentsID = -1;

  public:

    PolylineRenderer();

    void render(const entt::registry &, const Camera &);

  private:

    int nrOfSegments = 0;

    ivec2 segmentOffset = ivec2(0);
    void setSegmentOffset(const entt::registry &, entt::entity, const DrawPolyline &);

    /**
     * @param i                     index of the segment in the polyline
     * @param polylineSegments      nr of segments in the polyline
     */
    void addSegment(const DrawPolyline &, int i, int polylineSegments, ivec2 p0, ivec2 p1);
};


#endif
