
#include "LightMapRenderer.h"
#include "../../../level/ecs/components/physics/Physics.h"
#include "../../../level/ecs/components/graphics/Light.h"
#include "ShadowCaster.h"

LightMapRenderer::LightMapRenderer(Room *room)
    :
        room(room),
        shader("LightMapShader", "shaders/light/light_map.vert", "shaders/light/light_map.frag"),
        lightsData(
                VertAttributes().add_({"POS", 2}).add_({"RADIUS", 1}).add_({"SHADOW_TEXTURE_POS", 2, sizeof(int) * 2, GL_INT}),
                std::vector<u_char>()
        )
{
    quadMesh = Mesh::createQuad();
    VertBuffer::uploadSingleMesh(quadMesh);
    quadMesh->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}



LightMapRenderer::~LightMapRenderer()
{
    delete fbo;
}

void LightMapRenderer::render(const Camera &cam, const SharedTexture &shadowTexture)
{
    gu::profiler::Zone z("light map");
    if (!fbo || fbo->width != cam.viewportWidth || fbo->height != cam.viewportHeight)
    {
        delete fbo;
        fbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
        fbo->addDepthBuffer();
        fbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
    }

    fbo->bind();
    shader.use();
    glEnable(GL_DEPTH_TEST);
    uint zero = 0;
    glClearBufferuiv(GL_COLOR, 0, &zero);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    shadowTexture->bind(0, shader, "shadowTexture");

    TerrainCollisionDetector terrainCollisionDetector(room->getMap());

    int i = 0;
    lightsData.vertices.clear();
    room->entities.view<AABB, LightPoint>().each([&](AABB &aabb, LightPoint &light) {

        if (light.checkForTerrainCollision && terrainCollisionDetector.detect(aabb, true).anything)
            return;

        lightsData.addVertices(1);
        lightsData.set<vec2>(aabb.center, i, 0);
        lightsData.set<float>(light.radius + light.flickeringRadius, i, sizeof(vec2));

        ivec2 shadowTexturePos(-1);
        if (light.castShadow)
        {
            shadowTexturePos.x = light.shadowTextureIndex % (ShadowCaster::TEXTURE_SIZE / ShadowCaster::SIZE_PER_LIGHT);
            shadowTexturePos.y = light.shadowTextureIndex / (ShadowCaster::TEXTURE_SIZE / ShadowCaster::SIZE_PER_LIGHT);
            shadowTexturePos *= ivec2(ShadowCaster::SIZE_PER_LIGHT);
            shadowTexturePos += ivec2(ShadowCaster::SIZE_PER_LIGHT / 2);
        }
        lightsData.set<ivec2>(shadowTexturePos, i, sizeof(vec2) + sizeof(float));
        i++;
    });

    lightsDataBuffer = quadMesh->vertBuffer->uploadPerInstanceData(lightsData, 1, lightsDataBuffer);
    quadMesh->renderInstances(i);

    glDisable(GL_DEPTH_TEST);
    fbo->unbind();
}
