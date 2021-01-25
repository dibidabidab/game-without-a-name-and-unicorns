
#include "FireRenderer.h"
#include "../../../generated/Fire.hpp"
#include "../../../generated/Physics.hpp"
#include <graphics/3d/vert_buffer.h>
#include <generated/Spawning.hpp>

FireRenderer::FireRenderer()
    :
    particleShader("fireparticle shader", "shaders/fire/particle.vert", "shaders/fire/particle.frag"),
    combineShader("fire combine shader", "shaders/fullscreen_quad.vert", "shaders/fire/combine.frag"),
    particleData(
            VertAttributes()    // todo reduce byte-size:
                    .add_({"POS", 2})
                    .add_({"AGE", 1}),
            std::vector<u_char>()
    )
{
    quad = Mesh::createQuad();
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void FireRenderer::renderParticles(entt::registry &reg, const Camera &cam)
{
    particleData.vertices.clear(); // clear here, not at the end, because size is checked in renderCombined()
    int i = 0;
    reg.view<FireParticle, AABB, DespawnAfter>().each([&] (FireParticle &part, AABB &aabb, DespawnAfter &despawn) {
        particleData.addVertices(1);
        particleData.set(vec2(aabb.center), i, 0);
        particleData.set<float>(despawn.timer / despawn.time, i, sizeof(vec2));
        i++;
    });
    if (i == 0)
        return;

    particleDataID = quad->vertBuffer->uploadPerInstanceData(particleData, 1, particleDataID);

    fbo->bind();
    particleShader.use();

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glUniformMatrix4fv(particleShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    float zero = 0;
    glClearBufferfv(GL_COLOR, 0, &zero);

    quad->renderInstances(particleData.nrOfVertices());

    fbo->unbind();
    glDisable(GL_BLEND);
}

void FireRenderer::onResize(const Camera &cam)
{
    delete fbo;
    fbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
    fbo->addColorTexture(GL_R8, GL_RED, GL_NEAREST, GL_NEAREST);
}

void FireRenderer::renderCombined(float time, const Camera &cam)
{
    if (particleData.nrOfVertices() == 0)
        return;

    gu::profiler::Zone z("fire");

    combineShader.use();
    fbo->colorTexture->bind(0, combineShader, "particleMap");
    glUniform1f(combineShader.location("time"), time);
    glUniformMatrix4fv(combineShader.location("inverseProjection"), 1, GL_FALSE, &inverse(cam.combined)[0][0]);
    Mesh::getQuad()->render();
}
