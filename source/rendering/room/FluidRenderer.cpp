
#include "FluidRenderer.h"
#include "../../level/ecs/components/physics/Physics.h"
#include "../../level/ecs/components/Polyline.h"
#include <graphics/3d/vert_buffer.h>

FluidRenderer::FluidRenderer()
    :
    shader("Fluid shader", "shaders/fluids/trapezoid.vert", "shaders/fluids/trapezoid.frag"),
    segments(
        VertAttributes()
            .add_({"BOTTOM_Y", 1, 2, GL_SHORT})
            .add_({"LEFT_X", 1, 2, GL_SHORT})
            .add_({"RIGHT_X", 1, 2, GL_SHORT})
            .add_({"TOP_LEFT_Y", 1, 2, GL_SHORT})
            .add_({"TOP_RIGHT_Y", 1, 2, GL_SHORT})
            .add_({"SEGMENT_COLOR_INDEX", 1, 2, GL_SHORT}),
        std::vector<u_char>()
    )
{
    VertAttributes attrs;
    attrs.add({"POINT_INDEX", 1, 1, GL_BYTE});
    trapezoidMesh = SharedMesh(new Mesh("FluidTrapezoidMesh", 4, 6, attrs));
    trapezoidMesh->set<char>(0, 0, 0);
    trapezoidMesh->set<char>(1, 1, 0);
    trapezoidMesh->set<char>(2, 2, 0);
    trapezoidMesh->set<char>(3, 3, 0);
    trapezoidMesh->indices[0] = 0;
    trapezoidMesh->indices[1] = 1;
    trapezoidMesh->indices[2] = 2;
    trapezoidMesh->indices[3] = 2;
    trapezoidMesh->indices[4] = 3;
    trapezoidMesh->indices[5] = 0;
    VertBuffer::uploadSingleMesh(trapezoidMesh);
    trapezoidMesh->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void FluidRenderer::render(entt::registry &reg, const Camera &cam)
{
    gu::profiler::Zone z("fluids");

    segments.vertices.clear();

    nrOfSegments = 0;

    reg.view<Fluid, AABB, Polyline>().each([&](Fluid &fluid, AABB &aabb, Polyline &surfaceLine) {

        short bottomY = aabb.center.y - aabb.halfSize.y;

        auto it = surfaceLine.points.begin();

        while (it != --surfaceLine.points.end())
        {
            segments.addVertices(1);

            short
                leftX = it->x + aabb.center.x,
                topLeftY = it->y + aabb.center.y,
                rightX = (++it)->x + aabb.center.x,
                topRightY = it->y + aabb.center.y;

            segments.set<short>(bottomY, nrOfSegments, 0);
            segments.set<short>(leftX, nrOfSegments, 2);
            segments.set<short>(rightX, nrOfSegments, 4);
            segments.set<short>(topLeftY, nrOfSegments, 6);
            segments.set<short>(topRightY, nrOfSegments, 8);
            segments.set<short>(fluid.color, nrOfSegments, 10);

            nrOfSegments++;
        }
    });

    segmentsID = trapezoidMesh->vertBuffer->uploadPerInstanceData(segments, 1, segmentsID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    trapezoidMesh->renderInstances(nrOfSegments);
}
