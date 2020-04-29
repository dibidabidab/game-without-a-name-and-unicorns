
#include "ShadowCaster.h"
#include <gu/profiler.h>
#include <graphics/orthographic_camera.h>

ShadowCaster::ShadowCaster(Room *room)
    :
    room(room),
    debugShader("Shadow debug shader", "shaders/light/shadow.vert", "shaders/light/shadow_debug.frag"),
    shader("Shadow shader", "shaders/light/shadow.vert", "shaders/light/shadow.frag"),
    fbo(TEXTURE_SIZE, TEXTURE_SIZE)
{
    fbo.addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
    fbo.addDepthBuffer();

    auto attrs = VertAttributes().add_({"POS_2D", 2}).add_({"DEPTH", 1});

    shadowMesh = std::make_shared<Mesh>(
        "ShadowMesh",
        MAX_SHADOWS_PER_LIGHT * VERTS_PER_SHADOW,
        MAX_SHADOWS_PER_LIGHT * INDICES_PER_SHADOW,
        attrs
    );
    for (int i = 0; i < MAX_SHADOWS_PER_LIGHT; i++)
    {
        GLushort start = i * VERTS_PER_SHADOW;
        shadowMesh->indices.insert(shadowMesh->indices.begin() + i * INDICES_PER_SHADOW, {
            GLushort(start + 0), GLushort(start + 1), GLushort(start + 4),
            GLushort(start + 1), GLushort(start + 2), GLushort(start + 4),
            GLushort(start + 3), GLushort(start + 0), GLushort(start + 4),
            GLushort(start + 2), GLushort(start + 5), GLushort(start + 4),

            GLushort(start + 4), GLushort(start + 5), GLushort(start + 8),
            GLushort(start + 4), GLushort(start + 8), GLushort(start + 7),
            GLushort(start + 4), GLushort(start + 7), GLushort(start + 6),
            GLushort(start + 4), GLushort(start + 6), GLushort(start + 3)
        });
    }
    auto vb = VertBuffer::with(shadowMesh->attributes)->add(shadowMesh);
    vb->vboUsage = GL_DYNAMIC_DRAW;
    vb->upload(false);
}

void ShadowCaster::drawDebugLines(const Camera &cam)
{
    #ifndef EMSCRIPTEN // glPolygonMode() is not available in WebGL...
    gu::profiler::Zone z("shadow casting debug lines");

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    debugShader.use();
    glUniformMatrix4fv(debugShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    room->entities.view<AABB, LightPoint>().each([&](AABB &aabb, LightPoint &light) {
        if (!light.castShadow)
            return;

        updateMesh(light, aabb.center);

        shadowMesh->render();
    });
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif
}

void ShadowCaster::updateMesh(const LightPoint &light, const vec2 &lightPos)
{
    int i = 0;
    for (auto &outline : room->getMap().getOutlines())
    {
        const vec2 line[2] = {outline.first * vec2(16), outline.second * vec2(16)};

        bool render = false;
        float distance = length(line[0] - lightPos);
        if (distance <= light.radius)
            render = true;
        else
        {
            distance = length(line[1] - lightPos);
            render = distance <= light.radius;
        }

        if (!render)
            continue;

        vec2 lineNormal = line[1] - line[0];
        lineNormal = normalize(vec2(-lineNormal.y, lineNormal.x));

        vec2 diffToLine = ((line[0] + line[1]) * vec2(.5)) - lightPos;

        float angle = acos(dot(normalize(diffToLine), lineNormal)) * mu::RAD_TO_DEGREES;
        if (angle < 90)
            continue;

        vec2 avgDir(0);
        vec2 castLine[2];
        for (int j = 0; j < 2; j++)
        {
            vec2 diff = line[j] - lightPos;
            vec2 dir = normalize(diff);
            avgDir += dir;
            castLine[j] = line[j] + dir * vec2(light.radius);
        }
        avgDir = normalize(avgDir);

        vec2 diffToCastLine = ((castLine[0] + castLine[1]) * vec2(.5)) - lightPos;

        float castDepth = length(diffToLine - diffToCastLine);
        float additionalDepth = light.radius - castDepth;

        vec2 additionalLine[2]
        {
            castLine[0] + avgDir * vec2(additionalDepth),
            castLine[1] + avgDir * vec2(additionalDepth),
        };

        int vertI = i * VERTS_PER_SHADOW;

        shadowMesh->set<vec2>(line[0], vertI, 0);
        shadowMesh->set<float>(0, vertI++, sizeof(vec2));

        vec2 lineMid = (line[0] + line[1]) * vec2(.5);
        shadowMesh->set<vec2>(lineMid, vertI, 0);
        shadowMesh->set<float>(0, vertI++, sizeof(vec2));

        shadowMesh->set<vec2>(line[1], vertI, 0);
        shadowMesh->set<float>(0, vertI++, sizeof(vec2));



        shadowMesh->set<vec2>(castLine[0], vertI, 0);
        shadowMesh->set<float>(light.radius, vertI++, sizeof(vec2));

        vec2 castLineMid = (castLine[0] + castLine[1]) * vec2(.5);
        shadowMesh->set<vec2>(castLineMid, vertI, 0);
        shadowMesh->set<float>(length(castLineMid - lineMid), vertI++, sizeof(vec2));

        shadowMesh->set<vec2>(castLine[1], vertI, 0);
        shadowMesh->set<float>(light.radius, vertI++, sizeof(vec2));



        shadowMesh->set<vec2>(additionalLine[0], vertI, 0);
        shadowMesh->set<float>(length(additionalLine[0] - line[0]), vertI++, sizeof(vec2));

        vec2 additionalLineMid = (additionalLine[0] + additionalLine[1]) * vec2(.5);
        shadowMesh->set<vec2>(additionalLineMid, vertI, 0);
        shadowMesh->set<float>(length(additionalLineMid - lineMid), vertI++, sizeof(vec2));

        shadowMesh->set<vec2>(additionalLine[1], vertI, 0);
        shadowMesh->set<float>(length(additionalLine[1] - line[1]), vertI++, sizeof(vec2));

        if (++i == MAX_SHADOWS_PER_LIGHT)
            break;
    }
    shadowMesh->nrOfVertices = i * VERTS_PER_SHADOW;
    shadowMesh->nrOfIndices = i * INDICES_PER_SHADOW;
    shadowMesh->vertBuffer->reuploadVertices(shadowMesh);
}

void ShadowCaster::updateShadowTexture(const SharedTexture &tileMapTexture)
{
    gu::profiler::Zone z("shadow casting");
    fbo.bind();
    shader.use();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    OrthographicCamera cam(.1, 10, TEXTURE_SIZE, TEXTURE_SIZE);
    cam.position = mu::Z;
    cam.lookAt(mu::ZERO_3);

    tileMapTexture->bind(0, shader, "tileMapTexture");

    int i = -1;
    static const int LIGHTS_PER_ROW = TEXTURE_SIZE / SIZE_PER_LIGHT;

    room->entities.view<AABB, LightPoint>().each([&](AABB &aabb, LightPoint &light) {
        if (!light.castShadow)
        {
            light.shadowTextureIndex = -1;
            return;
        }
        i++;

        if (light.shadowTextureIndex == i && light.prevRadius >= light.radius && light.prevPosition == aabb.center)
            return;

        light.prevRadius = light.radius;
        light.prevPosition = aabb.center;
        light.shadowTextureIndex = i;

        updateMesh(light, aabb.center);

        ivec2 posOnTexture = ivec2(
            (i % LIGHTS_PER_ROW) * SIZE_PER_LIGHT,
            i / LIGHTS_PER_ROW * SIZE_PER_LIGHT
        );

        glScissor(posOnTexture.x, posOnTexture.y, SIZE_PER_LIGHT, SIZE_PER_LIGHT);
        uint zero = 0;
        glClearBufferuiv(GL_COLOR, 0, &zero);
        glClear(GL_DEPTH_BUFFER_BIT);

        posOnTexture -= ivec2(TEXTURE_SIZE / 2);
        posOnTexture += ivec2(SIZE_PER_LIGHT / 2);

        cam.position.x = aabb.center.x - posOnTexture.x;
        cam.position.y = aabb.center.y - posOnTexture.y;
        cam.update();
        glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
        shadowMesh->render();
    });

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    fbo.unbind();
}
