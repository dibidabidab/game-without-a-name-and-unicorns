
#include "PolylineRenderer.h"
#include "../level/ecs/components/VerletRope.h"
#include "../level/ecs/components/DrawPolyline.h"
#include "../level/ecs/components/BezierCurve.h"
#include "../level/ecs/components/Physics.h"
#include <graphics/3d/vert_buffer.h>

PolylineRenderer::PolylineRenderer()
    :
    shader("PolyLine shader", "shaders/polyline.vert", "shaders/polyline.frag"),
    lineSegments(
        VertAttributes()
                .add_({"POINT_A_X", 1, 2, GL_UNSIGNED_SHORT})
                .add_({"POINT_A_Y", 1, 2, GL_UNSIGNED_SHORT})
                .add_({"POINT_B_X", 1, 2, GL_UNSIGNED_SHORT})
                .add_({"POINT_B_Y", 1, 2, GL_UNSIGNED_SHORT})
                .add_({"SEGMENT_COLOR_INDEX", 1, 1, GL_UNSIGNED_BYTE})
                .add_({"Z_INDEX", 1}),
        std::vector<u_char>()
    )
{
    VertAttributes attrs;
    attrs.add({"POINT_INDEX", 1, 1, GL_BYTE});
    lineSegmentMesh = SharedMesh(new Mesh("PolyLineSegment", 2, 2, attrs));
    lineSegmentMesh->set<char>(0, 0, 0);
    lineSegmentMesh->set<char>(1, 1, 0);
    lineSegmentMesh->mode = GL_LINES;
    lineSegmentMesh->indices[0] = 0;
    lineSegmentMesh->indices[1] = 1;
    VertBuffer::uploadSingleMesh(lineSegmentMesh);
    lineSegmentMesh->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void PolylineRenderer::render(const entt::registry &reg, const Camera &cam)
{
    gu::profiler::Zone z("polylines");

    lineSegments.vertices.clear();

    nrOfSegments = 0;

    reg.view<const VerletRope, const DrawPolyline>().each([&](const VerletRope &rope, const DrawPolyline &draw) {

        if (rope.points.size() < 2)
            return;

        int nrOfRopeSegments = rope.points.size() - 1;

        lineSegments.addVertices(nrOfRopeSegments);

        for (int i = 0; i < nrOfRopeSegments; i++)
            addSegment(draw, i, nrOfRopeSegments, rope.points[i].currentPos, rope.points[i + 1].currentPos);
    });
    reg.view<const BezierCurve, const DrawPolyline>().each([&](const BezierCurve &curve, const DrawPolyline &draw) {
        if (curve.points.size() < 2 || curve.resolution < 1)
            return;

        int nrOfCurveSegments = curve.resolution;

        std::vector<vec2> points(curve.points.size());

        int i = 0;
        for (auto p : curve.points)
        {
            const AABB *aabb = reg.try_get<AABB>(p);
            if (!aabb)
                return;
            points[i++] = aabb->center;
        }
        lineSegments.addVertices(nrOfCurveSegments);

        vec2 prevPos = points[0];
        for (int i = 0; i < nrOfCurveSegments; i++)
        {
            float t = float(i + 1) / nrOfCurveSegments;
            vec2 newPos;

            if (points.size() == 3)
                mu::quadraticBezier(points[0], points[1], points[2], t, newPos);
            else if (points.size() == 4)
                mu::cubicBezier(points[0], points[1], points[2], points[3], t, newPos);
            else if (points.size() == 2)
                newPos = points[0] * (1.f - t) + points[1] * t;
            else
                std::cerr << "drawing of bezier curves with more than 4 points is not supported (yet)\n";

            addSegment(draw, i, nrOfCurveSegments, round(prevPos), round(newPos));

            prevPos = newPos;
        }
    });

    gu::profiler::Zone z2("draw calls");

    lineSegmentsID = lineSegmentMesh->vertBuffer->uploadPerInstanceData(lineSegments, 1, lineSegmentsID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    lineSegmentMesh->renderInstances(nrOfSegments);
}

void PolylineRenderer::addSegment(const DrawPolyline &draw, int i, int polylineSegments, ivec2 p0, ivec2 p1)
{
    lineSegments.set<uint16>(p0.x, nrOfSegments, 0);
    lineSegments.set<uint16>(p0.y, nrOfSegments, 2);
    lineSegments.set<uint16>(p1.x, nrOfSegments, 4);
    lineSegments.set<uint16>(p1.y, nrOfSegments, 6);

    uint8 color = 1u;

    if (!draw.colors.empty())
        color = draw.colors.at(i % draw.colors.size());

    lineSegments.set<uint8>(color, nrOfSegments, 8);

    float zIndex = polylineSegments == 1 ? 0 : i / float(polylineSegments - 1);
    zIndex = draw.zIndexBegin * zIndex + draw.zIndexEnd * (1. - zIndex);

    lineSegments.set<float>(zIndex, nrOfSegments, 9);

    nrOfSegments++;
}
