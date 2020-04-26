
#include "ShadowCaster.h"
#include <gu/profiler.h>

ShadowCaster::ShadowCaster(Room *room)
    :
    room(room),
    debugShader("Shadow debug shader", "shaders/shadow/shadow.vert", "shaders/shadow/shadow_debug.frag"),
    fbo(TEXTURE_SIZE, TEXTURE_SIZE)
{
    fbo.addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
    shadowMesh = std::make_shared<Mesh>(
        "ShadowMesh",
        MAX_SHADOWS_PER_LIGHT * VERTS_PER_SHADOW,
        MAX_SHADOWS_PER_LIGHT * INDICES_PER_SHADOW,
        VertAttributes().add_({"POS_2D", 2}).add_({"DEPTH", 1})
    );
    for (int i = 0; i < MAX_SHADOWS_PER_LIGHT; i++)
    {
        GLushort start = i * VERTS_PER_SHADOW;
        shadowMesh->indices.insert(shadowMesh->indices.begin() + i * INDICES_PER_SHADOW, {
            GLushort(start + 0), GLushort(start + 1), GLushort(start + 2),
            GLushort(start + 1), GLushort(start + 2), GLushort(start + 3),
            GLushort(start + 2), GLushort(start + 3), GLushort(start + 4),
            GLushort(start + 3), GLushort(start + 4), GLushort(start + 5)
        });
    }
    auto vb = VertBuffer::with(shadowMesh->attributes)->add(shadowMesh);
    vb->vboUsage = GL_DYNAMIC_DRAW;
    vb->upload(false);
}

void ShadowCaster::drawDebugLines(const Camera &cam)
{
    gu::profiler::Zone z("shadow casting");

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
        avgDir = normalize(avgDir); // isnt this the same as normalize(diffToLine) ??

        vec2 diffToCastLine = ((castLine[0] + castLine[1]) * vec2(.5)) - lightPos;

        float castDepth = length(diffToLine - diffToCastLine);
        float additionalDepth = light.radius - castDepth;

        vec2 additionalLine[2]
        {
            castLine[0] + avgDir * vec2(additionalDepth),
            castLine[1] + avgDir * vec2(additionalDepth),
        };

        assert(6 * 3 * sizeof(float) == shadowMesh->attributes.getVertSize() * VERTS_PER_SHADOW);

        shadowMesh->set<float[6 * 3]>({
              line[0].x, line[0].y, 0,
              line[1].x, line[1].y, 0,

              castLine[0].x, castLine[0].y, castDepth,
              castLine[1].x, castLine[1].y, castDepth,

              additionalLine[0].x, additionalLine[0].y, castDepth + additionalDepth,
              additionalLine[1].x, additionalLine[1].y, castDepth + additionalDepth,

        }, i * VERTS_PER_SHADOW, 0);

        if (++i == MAX_SHADOWS_PER_LIGHT)
            break;
    }
    shadowMesh->nrOfVertices = i * VERTS_PER_SHADOW;
    shadowMesh->nrOfIndices = i * INDICES_PER_SHADOW;
    shadowMesh->vertBuffer->reuploadVertices(shadowMesh);
}
