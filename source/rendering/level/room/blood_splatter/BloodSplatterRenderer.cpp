
#include <graphics/orthographic_camera.h>
#include <graphics/3d/vert_buffer.h>
#include "BloodSplatterRenderer.h"

#include "../../../../tiled_room/TiledRoom.h"
#include "../../../../generated/Physics.hpp"
#include "../../../../generated/BloodDrop.hpp"
#include <generated/Spawning.hpp>


BloodSplatterRenderer::BloodSplatterRenderer(TiledRoom *r)
    :
    room(r),
    fbo(
        r->getMap().width * TileMap::PIXELS_PER_TILE,
        r->getMap().height * TileMap::PIXELS_PER_TILE
    ),
    shader(
        "blood_splatter",
        "shaders/blood_splatter/blood_drop.vert",
        "shaders/blood_splatter/blood_drop.frag"
    ),
    instancedData(
        VertAttributes()    // todo reduce byte-size:
                .add_({"POS", 2})
                .add_({"SIZE", 1})
                .add_({"COLOR", 1, 4, GL_UNSIGNED_INT})
                .add_({"PERMANENT", 1, 4, GL_UNSIGNED_INT}),
        std::vector<u_char>()
    )
{
    fbo.addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);

    // set all pixels to 0, dont know if this is necessary:
    fbo.bind();
    uint zero = 0u;
    glClearBufferuiv(GL_COLOR, 0, &zero);
    fbo.unbind();

    quad = Mesh::createQuad(-1, 1);
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void BloodSplatterRenderer::render(const Camera &cam)
{
    gu::profiler::Zone z("blood splatter");
    if (nrOfDrops == 0)
        return;

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    glUniform1ui(shader.location("drawPermanent"), 0u);

    quad->renderInstances(nrOfDrops);
}

void BloodSplatterRenderer::updateSplatterTexture(float deltaTime)
{
    gu::profiler::Zone z("blood splatter");

    struct NewDrop {
        BloodDrop drop; AABB aabb; Physics physics;
    };
    std::list<NewDrop> newDrops;

    room->entities.view<BloodDrop, AABB, Physics>().each([&](auto e, BloodDrop &drop, AABB &aabb, Physics &physics) {
        physics.ghost = true;

        drop.timer += deltaTime;

        if (drop.permanentDrawOnTerrain && physics.touches.anything && drop.drawPermanentAfterTime == 0)
            drop.drawPermanentAfterTime = mu::random(.01, .2) + drop.timer;

        if (drop.drawPermanentAfterTime != 0 && drop.drawPermanentAfterTime < drop.timer && !drop.permanentDraw)
        {
            drop.permanentDraw = true;
            physics.gravity = 0;
            room->entities.assign<DespawnAfter>(e).time = mu::random(.3, 1);
        }

        if (!drop.split || drop.timer < drop.splitAtTime || drop.splitAtTime == 0 || drop.permanentDraw)
            return;

        int newOnes = mu::randomInt(1, 20);

        drop.size /= newOnes + 1;
        drop.splitAtTime = 0;

        for (int i = 0; i < newOnes; i++)
        {
            BloodDrop newDrop;
            newDrop.size = drop.size;
            newDrops.push_back({newDrop, aabb, physics});
            newDrops.back().physics.velocity += vec2(mu::random(-100, 100), mu::random(-100, 100));
        }
    });
    for (auto &newDrop : newDrops) // create actual entities outside of the view-loop, because entt is not clear on if this is safe
    {
        auto e = room->entities.create();
        room->entities.assign<BloodDrop>(e, newDrop.drop);
        room->entities.assign<AABB>(e, newDrop.aabb);
        room->entities.assign<Physics>(e, newDrop.physics);
    }


    // PREPARE RENDER DATA:
    instancedData.vertices.clear();

    nrOfDrops = 0;

    room->entities.view<BloodDrop, AABB, Physics>().each([&](BloodDrop &drop, AABB &aabb, Physics &physics) {
        instancedData.addVertices(1);
        int attrOffset = 0;
        instancedData.set<vec2>(aabb.center, nrOfDrops, attrOffset);
        attrOffset += sizeof(vec2);
        instancedData.set<float>(drop.size, nrOfDrops, attrOffset);
        attrOffset += sizeof(float);
        instancedData.set<uint>(drop.color, nrOfDrops, attrOffset);
        attrOffset += sizeof(uint);
        instancedData.set<uint>(drop.permanentDraw, nrOfDrops, attrOffset);
        nrOfDrops++;
    });
    if (nrOfDrops == 0)
        return;

    instancedDataID = quad->vertBuffer->uploadPerInstanceData(instancedData, 1, instancedDataID);

    fbo.bind();

    shader.use();

    OrthographicCamera cam(.1, 1000, fbo.width, fbo.height);
    cam.position = mu::Z;
    cam.lookAt(mu::ZERO_3);
    cam.position.x = cam.viewportWidth * .5;
    cam.position.y = cam.viewportHeight * .5;
    cam.update();

    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    glUniform1ui(shader.location("drawPermanent"), 1u);

    quad->renderInstances(nrOfDrops);
    fbo.unbind();
}
