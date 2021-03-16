
#include "PolylineRenderer.h"
#include "../generated/Polyline.hpp"
#include "../generated/Physics.hpp"
#include "../generated/VerletRope.hpp"
#include "../generated/DrawPolyline.hpp"
#include <graphics/3d/vert_buffer.h>

PolylineRenderer::PolylineRenderer()
    :
    shader("PolyLine shader", "shaders/polyline.vert", "shaders/polyline.frag"),
    wideLineShader("Wide polyLine shader", "shaders/wide_polyline.vert", "shaders/polyline.frag")
{
    VertAttributes attrs;
    attrs.add({"POINT_INDEX", 1, 1, GL_BYTE});
    lineSegmentMesh = SharedMesh(new Mesh("PolyLineSegment", 2, attrs));
    lineSegmentMesh->set<char>(0, 0, 0);
    lineSegmentMesh->set<char>(1, 1, 0);
    auto &part = lineSegmentMesh->parts.emplace_back();
    part.mode = GL_LINES;
    part.indices = {0, 1};
    VertBuffer::uploadSingleMesh(lineSegmentMesh);
    lineSegmentMesh->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;

    wideLineSegmentMesh = Mesh::createQuad(0, 1);
    VertBuffer::uploadSingleMesh(wideLineSegmentMesh);
    wideLineSegmentMesh->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void PolylineRenderer::render(const entt::registry &reg, const Camera &cam)
{
    gu::profiler::Zone z("polylines");

    currentWidth = 0;

    reg.view<const Polyline, const DrawPolyline>().each([&](auto e, const Polyline &line, const DrawPolyline &draw) {

        if (line.points.empty() || line.points.front() == line.points.back())
            return;
        setSegmentOffset(reg, e, draw);

        int nrOfLineSegments = line.points.size() - 1;

        auto it = line.points.begin();
        for (int i = 0; i < nrOfLineSegments; i++)
        {
            auto &p0 = *it;
            auto &p1 = *(++it);
            addSegment(draw, i, nrOfLineSegments, p0, p1);
        }
    });
    reg.view<const VerletRope, const DrawPolyline>().each([&](auto e, const VerletRope &rope, const DrawPolyline &draw) {

        if (rope.points.size() < 2)
            return;
        setSegmentOffset(reg, e, draw);

        int nrOfRopeSegments = rope.points.size() - 1;

        for (int i = 0; i < nrOfRopeSegments; i++)
            addSegment(draw, i, nrOfRopeSegments, rope.points[i].currentPos, rope.points[i + 1].currentPos);
    });
    reg.view<const BezierCurve, const DrawPolyline>().each([&](auto e, const BezierCurve &curve, const DrawPolyline &draw) {
        if (curve.points.size() < 2 || curve.resolution < 1)
            return;
        setSegmentOffset(reg, e, draw);

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

        vec2 prevPos = points[0];
        for (int i = 0; i < nrOfCurveSegments; i++)
        {
            float t = float(i + 1) / nrOfCurveSegments;
            vec2 newPos(0);

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

    for (auto &[width, segments] : widthSegmentsMap)
    {
        int i = segments.lineSegments.nrOfVertices();
        if (i == 0 || width == 0)
            continue;

        auto &shader = width == 1 ? this->shader : wideLineShader;

        shader.use();
        glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

        if (width != 1)
            glUniform1f(shader.location("halfWidth"), float(width) / 2.f);

        auto mesh = width == 1 ? lineSegmentMesh : wideLineSegmentMesh;
        segments.lineSegmentsID = mesh->vertBuffer->uploadPerInstanceData(segments.lineSegments, 1, segments.lineSegmentsID);
        mesh->renderInstances(i);
        segments.lineSegments.vertices.clear();
    }
}

void PolylineRenderer::addSegment(const DrawPolyline &draw, int i, int polylineSegments, ivec2 p0, ivec2 p1)
{
    p0 += segmentOffset;
    p1 += segmentOffset;

    uint8 color = 1u;

    if (!draw.colors.empty())
        color = draw.colors.at(i % draw.colors.size());

    float zIndex0 = i / float(polylineSegments);
    zIndex0 = draw.zIndexBegin + zIndex0 * (draw.zIndexEnd - draw.zIndexBegin);

    float zIndex1 = (i + 1.f) / float(polylineSegments);
    zIndex1 = draw.zIndexBegin + zIndex1 * (draw.zIndexEnd - draw.zIndexBegin);

    int repeatI = 0;
    for (int xRepeat = 0; xRepeat < 1 + draw.repeatX; xRepeat++)
    {
        for (int yRepeat = 0; yRepeat < 1 + draw.repeatY; yRepeat++)
        {
            if (!draw.repeatColors.empty())
            {
                color = draw.repeatColors.at(repeatI % draw.repeatColors.size());
            }

            lineSegments->addVertices(1);
            lineSegments->set<int16>(p0.x, nrOfSegments, 0);
            lineSegments->set<int16>(p0.y, nrOfSegments, 2);
            lineSegments->set<int16>(zIndex0, nrOfSegments, 4);
            lineSegments->set<int16>(p1.x, nrOfSegments, 6);
            lineSegments->set<int16>(p1.y, nrOfSegments, 8);
            lineSegments->set<int16>(zIndex1, nrOfSegments, 10);

            lineSegments->set<uint32>(color, nrOfSegments, 12);

            nrOfSegments++;
            if (draw.repeatY > 0)
            {
                p0.y--;
                p1.y--;
            }
            repeatI++;
        }
        p0.x++;
        p1.x++;
    }
}

void PolylineRenderer::setSegmentOffset(const entt::registry &reg, entt::entity e, const DrawPolyline &draw)
{
    currentWidth = draw.lineWidth;
    lineSegments = &(widthSegmentsMap[currentWidth].lineSegments);
    nrOfSegments = lineSegments->nrOfVertices();

    segmentOffset = ivec2(0);
    if (draw.addAABBOffset)
    {
        auto *aabb = reg.try_get<AABB>(e);
        if (aabb)
            segmentOffset += aabb->center;
    }
}

PolylineRenderer::Segments::Segments()
    :
    lineSegments(
        VertAttributes()
            .add_({"POINT_A_X", 1, 2, GL_SHORT})
            .add_({"POINT_A_Y", 1, 2, GL_SHORT})
            .add_({"POINT_A_Z", 1, 2, GL_SHORT})
            .add_({"POINT_B_X", 1, 2, GL_SHORT})
            .add_({"POINT_B_Y", 1, 2, GL_SHORT})
            .add_({"POINT_B_Z", 1, 2, GL_SHORT})
            .add_({"SEGMENT_COLOR_INDEX", 1, 4, GL_UNSIGNED_INT}),
        std::vector<u_char>()
    )
{

}
