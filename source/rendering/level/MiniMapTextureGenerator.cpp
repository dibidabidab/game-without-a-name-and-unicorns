
#include "MiniMapTextureGenerator.h"
#include "../../tiled_room/TiledRoom.h"

ivec2 miniMapSize(const Level &lvl)
{
    ivec2 size(0);

    for (int i = 0; i < lvl.getNrOfRooms(); i++)
    {
        auto &room = (TiledRoom &) lvl.getRoom(i);

        size.x = max<int>(size.x, room.positionInLevel.x + room.getMap().width);
        size.y = max<int>(size.y, room.positionInLevel.y + room.getMap().height);
    }

    return size;
}

SharedTexture MiniMapTextureGenerator::generate(const Level &lvl)
{
    auto size = miniMapSize(lvl);
    if (size.x * size.y > 2048*2048)
    {
        std::cerr << "Tried to make a MiniMap-Texture for a HUGEEEEE Level-map.\nPlease check all Rooms and move them closer to (0, 0) if needed." << std::endl;
        return std::make_shared<Texture>(-1, 0, 0);
    }

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    std::vector<u8vec4> imgData(size.x * size.y, u8vec4(0));

    for (int i = 0; i < lvl.getNrOfRooms(); i++)
    {
        auto &room = (TiledRoom &) lvl.getRoom(i);
        auto &map = room.getMap();

        TileMaterial prevMaterial = -1;
        u8vec4 materialColor;

        for (int w = 0; w < map.width; w++)
        {
            for (int h = 0; h < map.height; h++)
            {
                if (map.getTile(w, h) == Tile::empty)
                    continue;

                TileMaterial mat = map.getMaterial(w, h);
                if (mat != prevMaterial)
                {
                    mu::floatColorToByteVec(vec4(map.getMaterialProperties(mat).color, 1), materialColor);
                    prevMaterial = mat;
                }
                int x = room.positionInLevel.x + w;
                int y = room.positionInLevel.y + h;
                y = size.y - 1 - y;

                imgData[y * size.x + x] = materialColor;
            }
        }
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imgData[0]);


    return std::make_shared<Texture>(texId, size.x, size.y);
}
