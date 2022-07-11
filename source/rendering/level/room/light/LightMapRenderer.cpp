
#include "LightMapRenderer.h"
#include "../../../../generated/Physics.hpp"
#include "../../../../generated/Light.hpp"
#include "ShadowCaster.h"
#include "../../../../ecs/systems/physics/TerrainCollisionDetector.h"
#include <level/Level.h>

LightMapRenderer::LightMapRenderer(TiledRoom *room)
    :
        room(room),
        pointLightShader("PointLightShader", "shaders/light/point_light.vert", "shaders/light/point_light.frag"),
        pointLightsData(
            VertAttributes()
                .add_({"POS", 2})
                .add_({"RADIUS", 1})
                .add_({"SHADOW_TEXTURE_POS", 2, sizeof(int) * 2, GL_INT}),
            std::vector<u_char>()
        ),
        directionalLightShader("DirectionalLightShader", "shaders/light/directional_light.vert", "shaders/light/directional_light.frag"),
        directionalLightsData(
            VertAttributes()
                .add_({"POS", 2})
                .add_({"WIDTH", 1})
                .add_({"DISTANCE", 1})
                .add_({"ROTATION", 1}),
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
    fbo->bind();
    glEnable(GL_DEPTH_TEST);
    glClearBufferuiv(GL_COLOR, 0, &room->baseLightLevel);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (room->baseLightLevel < 2u)
    {
        renderDirectionalLights(cam);
        renderPointLights(cam, shadowTexture);
    }

    glDisable(GL_DEPTH_TEST);
    fbo->unbind();
}

void LightMapRenderer::renderPointLights(const Camera &cam, const SharedTexture &shadowTexture)
{

    pointLightShader.use();
    glUniformMatrix4fv(pointLightShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    shadowTexture->bind(0, pointLightShader, "shadowTexture");

    TerrainCollisionDetector terrainCollisionDetector(room->getMap());

    int i = 0;
    pointLightsData.vertices.clear();
    room->entities.view<AABB, PointLight>().each([&](AABB &aabb, PointLight &light) {

        if (light.checkForTerrainCollision)
        {
            TerrainCollisions cols;
            terrainCollisionDetector.detect(cols, aabb, true);
            if (cols.anything)
                return;
        }

        pointLightsData.addVertices(1);
        pointLightsData.set<vec2>(aabb.center, i, 0);
        pointLightsData.set<float>(light.radius + light.flickeringRadius, i, sizeof(vec2));

        ivec2 shadowTexturePos(-1);
        if (light.castShadow)
        {
            shadowTexturePos.x = light.shadowTextureIndex % (ShadowCaster::TEXTURE_SIZE / ShadowCaster::SIZE_PER_LIGHT);
            shadowTexturePos.y = light.shadowTextureIndex / (ShadowCaster::TEXTURE_SIZE / ShadowCaster::SIZE_PER_LIGHT);
            shadowTexturePos *= ivec2(ShadowCaster::SIZE_PER_LIGHT);
            shadowTexturePos += ivec2(ShadowCaster::SIZE_PER_LIGHT / 2);
        }
        pointLightsData.set<ivec2>(shadowTexturePos, i, sizeof(vec2) + sizeof(float));
        i++;
    });

    pointLightsDataBuffer = quadMesh->vertBuffer->uploadPerInstanceData(pointLightsData, 1, pointLightsDataBuffer);
    quadMesh->renderInstances(i);
}

void LightMapRenderer::renderDirectionalLights(const Camera &cam)
{
    int i = 0;
    directionalLightsData.vertices.clear();
    room->entities.view<AABB, DirectionalLight>().each([&](AABB &aabb, DirectionalLight &light) {

        directionalLightsData.addVertices(1);

        directionalLightsData.set<vec2>(aabb.center, i, 0);
        directionalLightsData.set<float>(light.width, i, sizeof(vec2));
        directionalLightsData.set<float>(light.distance, i, sizeof(vec2) + sizeof(float));
        directionalLightsData.set<float>(light.rotation * mu::DEGREES_TO_RAD, i, sizeof(vec2) + sizeof(float) * 2);

        i++;
    });
    if (i == 0)
        return;

    directionalLightShader.use();
    asset<Texture>("textures/light_shafts")->bind(0, directionalLightShader, "lightShaftsTexture");
    glUniform1f(directionalLightShader.location("time"), room->getLevel().getTime());
    glUniformMatrix4fv(directionalLightShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    directionalLightsDataBuffer = quadMesh->vertBuffer->uploadPerInstanceData(directionalLightsData, 1, directionalLightsDataBuffer);
    quadMesh->renderInstances(i);
}

void LightMapRenderer::onResize(const Camera &cam)
{
    delete fbo;
    fbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
    fbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST, GL_UNSIGNED_BYTE);
    fbo->addDepthBuffer();
}
