
#include "SpriteRenderer.h"
#include "../../game/Game.h"
#include <graphics/3d/vert_buffer.h>

SpriteRenderer::SpriteRenderer()
    :
    shader("sprite shader", "shaders/sprite.vert", "shaders/sprite.frag"),
    instancedData(
        VertAttributes()    // todo reduce byte-size:
            .add_({"POS", 3})
            .add_({"SIZE", 2})
            .add_({"TEXTURE_OFFSET", 2})
            .add_({"ROTATE_90_DEG", 1, 4, GL_INT})

            .add_({"CLIP_LOWER_LEFT_X", 1, 4, GL_INT})
            .add_({"CLIP_LOWER_LEFT_Y", 1, 4, GL_INT})

            .add_({"CLIP_LOWER_RIGHT_X", 1, 4, GL_INT})
            .add_({"CLIP_LOWER_RIGHT_Y", 1, 4, GL_INT})

            .add_({"CLIP_TOP_LEFT_X", 1, 4, GL_INT})
            .add_({"CLIP_TOP_LEFT_Y", 1, 4, GL_INT})

            .add_({"CLIP_TOP_RIGHT_X", 1, 4, GL_INT})
            .add_({"CLIP_TOP_RIGHT_Y", 1, 4, GL_INT}),
        std::vector<u_char>()
    )
{
    quad = Mesh::createQuad(0, 1);
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void SpriteRenderer::renderUsingAABBs(const Camera &cam, entt::registry &reg)
{
    gu::profiler::Zone z("sprites render");

    reg.view<AsepriteView, AABB>().each([&](AsepriteView &view, const AABB &aabb) {

        if (!view.sprite.isSet())
            return;

        ivec2 position = view.getDrawPosition(aabb);
        add(view, position);
    });
    render(cam);
}

void SpriteRenderer::add(const AsepriteView &view, const ivec2 &position)
{
    ivec2 frameOffset = Game::spriteSheet.spriteInfo(view.sprite.get()).frameOffsets.at(view.frame);

    vec2 size(view.sprite->width, view.sprite->height);
    if (view.flipHorizontal)
    {
        frameOffset.x += view.sprite->width;
        size.x *= -1;
    }
    if (view.flipVertical)
    {
        frameOffset.y += view.sprite->height;
        size.y *= -1;
    }

    int i = instancedData.nrOfVertices();
    instancedData.addVertices(1);
    int attrOffset = 0;
    instancedData.set<vec3>(vec3(position, view.zIndex), i, attrOffset);
    attrOffset += sizeof(vec3);
    instancedData.set<vec2>(size, i, attrOffset);
    attrOffset += sizeof(vec2);
    instancedData.set<vec2>(frameOffset, i, attrOffset);
    attrOffset += sizeof(vec2);
    instancedData.set<int>(view.rotate90Deg ? 1 : 0, i, attrOffset);
    attrOffset += sizeof(int);

    instancedData.set<ivec2>(view.clip.lowerLeft, i, attrOffset);
    attrOffset += sizeof(ivec2);
    instancedData.set<ivec2>(view.clip.lowerRight, i, attrOffset);
    attrOffset += sizeof(ivec2);
    instancedData.set<ivec2>(view.clip.topLeft, i, attrOffset);
    attrOffset += sizeof(ivec2);
    instancedData.set<ivec2>(view.clip.topRight, i, attrOffset);
}

void SpriteRenderer::render(const Camera &cam)
{
    gu::profiler::Zone z2("sprite draw calls");

    instancedDataID = quad->vertBuffer->uploadPerInstanceData(instancedData, 1, instancedDataID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    Game::spriteSheet.texture->bind(0, shader, "spriteSheet");

    quad->renderInstances(instancedData.nrOfVertices());
    instancedData.vertices.clear();
}
