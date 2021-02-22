
#include "NineSliceRenderer.h"
#include "../../game/Game.h"
#include <graphics/3d/vert_buffer.h>

NineSliceRenderer::NineSliceRenderer()
    :
    shader("9slice shader", "shaders/ui/9slice.vert", "shaders/ui/9slice.frag"),
    instancedData(
        VertAttributes()
            .add_({"POS", 3, 6, GL_SHORT})
            .add_({"SIZE", 2, 4, GL_SHORT})
            .add_({"SLICE_SIZE", 2, 2, GL_UNSIGNED_BYTE})
            .add_({"SLICE_OFFSET_ON_SHEET", 2, 4, GL_UNSIGNED_SHORT})
            .add_({"TOP_LEFT_OFFSET", 2, 2, GL_UNSIGNED_BYTE})
            .add_({"INNER_SIZE", 2, 2, GL_UNSIGNED_BYTE}),
        std::vector<u_char>()
    )
{
    quad = Mesh::createQuad(0, 1);
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void NineSliceRenderer::render(const Camera &cam)
{
    gu::profiler::Zone z2("9slice draw calls");

    instancedDataID = quad->vertBuffer->uploadPerInstanceData(instancedData, 1, instancedDataID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    Game::spriteSheet.texture->bind(0, shader, "spriteSheet");

    quad->renderInstances(instancedData.nrOfVertices());
    instancedData.vertices.clear();
}

void NineSliceRenderer::add(const aseprite::Sprite &sprite, const i16vec3 &position, const ivec2 &size, uint16 frame)
{
    auto &slice = sprite.getSliceByName("9slice", 0);

    if (!slice.nineSlice.has_value())
        throw gu_err("The '9slice' slice of " + sprite.name + " is not an actual NineSlice!");

    auto &nineSlice = slice.nineSlice.value();

    auto &frameOffsets = Game::spriteSheet.spriteInfo(sprite).frameOffsets;

    if (frame >= frameOffsets.size())
        throw gu_err("Frame #" + std::to_string(frame) + " not available for sprite " + sprite.name);

    u16vec2 offset = frameOffsets.at(frame);
    offset.x += slice.originX;
    offset.y += sprite.height - (slice.originY + slice.height);

    int i = instancedData.nrOfVertices();
    instancedData.addVertices(1);

    instancedData.set(i16vec3(position), i, 0);
    int attrOffset = sizeof(i16vec3);
    instancedData.set(i16vec2(size), i, attrOffset);
    attrOffset += sizeof(i16vec2);
    instancedData.set(u8vec2(slice.width, slice.height), i, attrOffset);
    attrOffset += sizeof(u8vec2);
    instancedData.set(offset, i, attrOffset);
    attrOffset += sizeof(u16vec2);
    instancedData.set(u8vec2(nineSlice.topLeftOffset), i, attrOffset);
    attrOffset += sizeof(u8vec2);
    instancedData.set(u8vec2(nineSlice.innerSize), i, attrOffset);
//    attrOffset += sizeof(u8vec2);
}
