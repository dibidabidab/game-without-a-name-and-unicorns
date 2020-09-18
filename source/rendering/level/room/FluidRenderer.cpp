
#include "FluidRenderer.h"
#include "../../../generated/Physics.hpp"
#include "../../../generated/Polyline.hpp"
#include <graphics/3d/vert_buffer.h>

FluidRenderer::FluidRenderer()
    :
    shader("Fluid shader", "shaders/fluids/trapezoid.vert", "shaders/fluids/trapezoid.frag"),
    reflectionsShader("Fluid Reflections shader", "shaders/fluids/reflectionTrapezoid.vert", "shaders/fluids/reflectionTrapezoid.frag"),
    segments(
        VertAttributes()
            .add_({"SEGMENT_COLOR_INDEX", 1, 2, GL_SHORT})
            .add_({"BOTTOM_Y", 1, 2, GL_SHORT})
            .add_({"LEFT_X", 1, 2, GL_SHORT})
            .add_({"RIGHT_X", 1, 2, GL_SHORT})
            .add_({"TOP_LEFT_Y", 1, 4, GL_FLOAT})
            .add_({"TOP_RIGHT_Y", 1, 4, GL_FLOAT}),
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

            short leftX = it->x + aabb.center.x;
            float topLeftY = it->y + aabb.center.y;
            short rightX = (++it)->x + aabb.center.x;
            float topRightY = it->y + aabb.center.y;

            segments.set<short>(fluid.color, nrOfSegments, 0);
            segments.set<short>(bottomY, nrOfSegments, 2);
            segments.set<short>(leftX, nrOfSegments, 4);
            segments.set<short>(rightX, nrOfSegments, 6);
            segments.set<float>(topLeftY, nrOfSegments, 8);
            segments.set<float>(topRightY, nrOfSegments, 12);

            nrOfSegments++;
        }
    });
    if (nrOfSegments == 0)
        return;

    segmentsID = trapezoidMesh->vertBuffer->uploadPerInstanceData(segments, 1, segmentsID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    trapezoidMesh->renderInstances(nrOfSegments);
}

void FluidRenderer::onResize(const Camera &cam)
{
    delete reflectionsFbo;
    reflectionsFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
    reflectionsFbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
}

void FluidRenderer::renderReflections(FrameBuffer *indexedImage, const Camera &cam, float time)
{
    gu::profiler::Zone z("fluid reflections");

    reflectionsFbo->bind();

    uint zero = 0;
    glClearBufferuiv(GL_COLOR, 0, &zero);

    if (nrOfSegments != 0)
    {
        reflectionsShader.use();
        glUniformMatrix4fv(reflectionsShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
        glUniform1f(reflectionsShader.location("time"), time);
        indexedImage->colorTexture->bind(1, reflectionsShader, "indexedImage");
        indexedImage->depthTexture->bind(2, reflectionsShader, "indexedImageDepth");
        trapezoidMesh->renderInstances(nrOfSegments);
    }

    reflectionsFbo->unbind();
}
