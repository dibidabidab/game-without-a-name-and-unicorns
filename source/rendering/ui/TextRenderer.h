
#ifndef GAME_TEXTRENDERER_H
#define GAME_TEXTRENDERER_H


#include <graphics/shader_asset.h>
#include <graphics/3d/mesh.h>
#include <graphics/camera.h>
#include "../../ecs/components/ui/UIComponents.h"

class TextRenderer
{

    SharedMesh quad;
    VertData instancedData;
    int instancedDataID = -1;

    ShaderAsset shader;

    struct FontData
    {
        int minLineHeight = 0;
        asset<aseprite::Sprite> sprite;
        ivec2 spriteOffsetOnMegaSpriteSheet;
        std::vector<const aseprite::Slice *> charSlices; // charSlices[0] = space, same as charSlices[' ' - 32]. see http://www.asciitable.com/
    };

    std::unordered_map<std::string, FontData> fontDatas;

  public:

    TextRenderer();

    void add(const TextView &, int lineX, ivec2 &cursor, int &currLineHeight);

    void render(const Camera &);

};


#endif
