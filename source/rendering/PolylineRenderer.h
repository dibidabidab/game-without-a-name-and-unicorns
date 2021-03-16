
#ifndef GAME_POLYLINERENDERER_H
#define GAME_POLYLINERENDERER_H


#include <graphics/3d/mesh.h>
#include <graphics/shader_asset.h>
#include <graphics/camera.h>
#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../generated/DrawPolyline.hpp"

class PolylineRenderer
{

    ShaderAsset shader, wideLineShader;

    SharedMesh lineSegmentMesh, wideLineSegmentMesh;

    struct Segments
    {
        Segments();

        VertData lineSegments;
        int lineSegmentsID = -1;
    };

    std::unordered_map<uint8, Segments> widthSegmentsMap;

  public:

    PolylineRenderer();

    void render(const entt::registry &, const Camera &);

  private:

    uint8 currentWidth = 0;
    VertData *lineSegments = NULL; // lineSegments of current width.
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
