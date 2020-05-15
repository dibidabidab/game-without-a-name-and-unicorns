
#include "SpriteRenderer.h"
#include "../../level/ecs/components/Physics.h"
#include <graphics/3d/vert_buffer.h>

SpriteRenderer::SpriteRenderer(const MegaSpriteSheet *s)
    :
    megaSpriteSheet(s),
    shader("sprite shader", "shaders/sprite.vert", "shaders/sprite.frag"),
    instancedData(
        VertAttributes()
            .add_({"POS", 3})
            .add_({"SIZE", 2})
            .add_({"TEXTURE_OFFSET", 2}),
        std::vector<u_char>()
    )
{
    quad = Mesh::createQuad(0, 1);
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void SpriteRenderer::render(double deltaTime, const Camera &cam, entt::registry &reg)
{
    gu::profiler::Zone z("sprites render");

    instancedData.vertices.clear();

    int i = 0;

    reg.view<AsepriteView, AABB>().each([&](AsepriteView &view, const AABB &aabb) {

        if (view.frame >= view.sprite->frameCount)
            view.frame = 0;

        if (view.playingTag >= view.sprite->tags.size())
            view.playingTag = -1;

        if (view.playingTag >= 0)
            updateAnimation(view, deltaTime);

        ivec2 frameOffset = megaSpriteSheet->spriteInfo(view.sprite.get()).frameOffsets.at(view.frame);

        instancedData.addVertices(1);

        ivec2 position = view.getDrawPosition(aabb);

        instancedData.set<vec3>(vec3(position, view.zIndex), i, 0);
        instancedData.set<vec2>(vec2(view.sprite->width, view.sprite->height), i, sizeof(vec3));
        instancedData.set<vec2>(frameOffset, i, sizeof(vec3) + sizeof(vec2));

        i++;
    });
    gu::profiler::Zone z2("draw calls");

    instancedDataID = quad->vertBuffer->uploadPerInstanceData(instancedData, 1, instancedDataID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    megaSpriteSheet->texture->bind(0, shader, "spriteSheet");

    quad->renderInstances(i);
}

void SpriteRenderer::updateAnimation(AsepriteView &view, double deltaTime)
{
    if (!view.paused)
        view.frameTimer += deltaTime;

    const auto *frame = &view.sprite->frames.at(view.frame);

    while (view.frameTimer >= frame->duration)
    {
        view.frameTimer -= frame->duration;

        const auto &tag = view.sprite->tags[view.playingTag];

        if (tag.loopDirection == aseprite::Tag::pingPong)
        {
            if (view.pong && view.frame-- <= tag.from)
            {
                view.pong = false;
                view.frame = tag.from == tag.to ? tag.from : tag.from + 1;
            }
            else if (!view.pong && view.frame++ >= tag.to)
            {
                view.pong = true;
                view.frame = tag.from == tag.to ? tag.from : tag.to - 1;
            }
        }
        else if (tag.loopDirection == aseprite::Tag::reverse)
        {
            if (view.frame-- <= tag.from)
                view.frame = tag.to;
        }
        else if (view.frame++ >= tag.to)
            view.frame = tag.from;
    }
}
