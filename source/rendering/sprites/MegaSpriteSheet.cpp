
#include <utils/gu_error.h>
#include "MegaSpriteSheet.h"

void MegaSpriteSheet::add(const aseprite::Sprite &sprite)
{
    if (!texture)
        createTexture();
    texture->bind(0);

    ivec2 chunkSize(
        ceil(vec2(sprite.width, sprite.height) / float(CHUNK_SIZE))
    );

    SubSheet subSheet;

    for (auto &frame : sprite.frames)
    {
        ivec2 chunkOffset(0);
        bool foundPlace = false;

        for (chunkOffset.x = 0; chunkOffset.x <= CHUNKS_PER_ROW - chunkSize.x; chunkOffset.x++)
        {
            for (chunkOffset.y = 0; chunkOffset.y <= CHUNKS_PER_ROW - chunkSize.y; chunkOffset.y++)
            {
                if (tryReserve(chunkOffset, chunkSize))
                {
                    foundPlace = true;
                    break;
                }
            }
            if (foundPlace)
                break;
        }
        if (!foundPlace)
            throw gu_err("MegaSpriteSheet is too small....");

        glTexSubImage2D(
                GL_TEXTURE_2D, 0, chunkOffset.x * CHUNK_SIZE, chunkOffset.y * CHUNK_SIZE, sprite.width, sprite.height,
                GL_RED_INTEGER, GL_UNSIGNED_BYTE, &frame.pixels[0]
        );
        subSheet.frameOffsets.push_back(chunkOffset * CHUNK_SIZE);
    }
    subSheets[&sprite] = subSheet;
}

bool MegaSpriteSheet::tryReserve(const ivec2 &chunkOffset, const ivec2 &chunkSize)
{
    for (int x = chunkOffset.x; x < chunkOffset.x + chunkSize.x; x++)
        for (int y = chunkOffset.y; y <= chunkOffset.y + chunkSize.y; y++)
            if (used[x][y])
                return false;
    for (int x = chunkOffset.x; x < chunkOffset.x + chunkSize.x; x++)
        for (int y = chunkOffset.y; y <= chunkOffset.y + chunkSize.y; y++)
            used[x][y] = true;

    return true;
}

void MegaSpriteSheet::createTexture()
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, SIZE, SIZE, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    texture = std::make_shared<Texture>(textureID, SIZE, SIZE);
}

const MegaSpriteSheet::SubSheet &MegaSpriteSheet::spriteInfo(const aseprite::Sprite &s) const
{
    return subSheets.at(&s);
}

void MegaSpriteSheet::printUsage() const
{
    float usedChunks = 0;
    for (int x = 0; x < CHUNKS_PER_ROW; x++)
        for (int y = 0; y < CHUNKS_PER_ROW; y++)
            if (used[x][y])
                usedChunks++;
    int usage = round(100.f * usedChunks / float(CHUNKS_PER_ROW * CHUNKS_PER_ROW));
    std::cout << usage << "% used of MegaSpriteSheet\n";
}
