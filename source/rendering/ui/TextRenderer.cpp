
#include "TextRenderer.h"
#include <graphics/3d/vert_buffer.h>
#include "../../game/Game.h"

TextRenderer::TextRenderer()
    :
    shader("text shader", "shaders/ui/text.vert", "shaders/ui/text.frag"),
    instancedData(
        VertAttributes()
            .add_({"POS", 3, 6, GL_SHORT})
            .add_({"SPRITE_OFFSET", 2, 4, GL_UNSIGNED_SHORT})
            .add_({"SIZE", 2, 2, GL_UNSIGNED_BYTE})
            .add_({"MAP_COLOR_FROM", 1, 1, GL_UNSIGNED_BYTE})
            .add_({"MAP_COLOR_TO", 1, 1, GL_UNSIGNED_BYTE}),
        std::vector<u_char>()
    )
{
    quad = Mesh::createQuad(0, 1);
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}


void TextRenderer::add(const TextView &textView, int lineX, ivec2 &cursor, int &currLineHeight)
{
    auto &fontData = fontDatas[textView.fontSprite.getLoadedAsset().shortPath];
    if (fontData.charSlices.empty() || fontData.sprite.hasReloaded())
    {
        // initialize fontData:
        fontData.charSlices.clear();

        fontData.sprite = textView.fontSprite;
        fontData.spriteOffsetOnMegaSpriteSheet = Game::spriteSheet.spriteInfo(fontData.sprite.get()).frameOffsets.at(0);

        for (char c = ' '; c <= '~'; c++)   // http://www.asciitable.com/
        {
            auto &slice = fontData.sprite->getSliceByName(std::string(1, c).c_str(), 0);
            fontData.charSlices.push_back(&slice);

            int charHeight = slice.height;
            charHeight -= slice.pivot.has_value() ? slice.pivot.value().y : 0;
            fontData.minLineHeight = max(fontData.minLineHeight, charHeight);
        }
    }
    currLineHeight = max(fontData.minLineHeight, currLineHeight);

    for (char c : textView.text)
    {
        if (c == '\n')
        {
            cursor.x = lineX;
            cursor.y -= fontData.minLineHeight + textView.lineSpacing;
            continue;
        }

        int sliceIndex = c - ' ';
        if (sliceIndex < 0 || sliceIndex >= fontData.charSlices.size())
            throw gu_err("Tried to render character: '" + std::string(1, c) + "'");

        auto slice = fontData.charSlices.at(sliceIndex);

        int vertI = instancedData.nrOfVertices();
        instancedData.addVertices(1);

        using posType = i16vec3;
        using spriteOffsetType = u16vec2;
        using sizeType = u8vec2;

        int attrOffset = 0;

        int yOffset = slice->pivot.has_value() ? -slice->pivot.value().y : 0;
        instancedData.set(posType(cursor.x, cursor.y + yOffset + currLineHeight, 0), vertI, attrOffset);
        attrOffset += sizeof(posType);

        spriteOffsetType spriteOffset(fontData.spriteOffsetOnMegaSpriteSheet);
        spriteOffset.x += slice->originX;
        spriteOffset.y += slice->originY;

        instancedData.set(spriteOffset, vertI, attrOffset);
        attrOffset += sizeof(spriteOffsetType);
        instancedData.set(sizeType(slice->width, slice->height), vertI, attrOffset);
        attrOffset += sizeof(sizeType);
        instancedData.set(u8vec2(textView.mapColorFrom, textView.mapColorTo), vertI, attrOffset);
//        attrOffset += sizeof(u8vec2);

        cursor.x += slice->width + textView.letterSpacing;
    }
}

void TextRenderer::render(const Camera &cam)
{
    if (instancedData.vertices.empty())
        return;

    instancedDataID = quad->vertBuffer->uploadPerInstanceData(instancedData, 1, instancedDataID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);
    Game::spriteSheet.texture->bind(0, shader, "spriteSheet");

    quad->renderInstances(instancedData.nrOfVertices());

    instancedData.vertices.clear();
}
