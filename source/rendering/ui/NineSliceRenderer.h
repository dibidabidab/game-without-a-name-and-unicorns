
#ifndef GAME_NINESLICERENDERER_H
#define GAME_NINESLICERENDERER_H

#include <graphics/3d/mesh.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include <utils/aseprite/Aseprite.h>

class NineSliceRenderer
{

    SharedMesh quad;
    VertData instancedData;
    int instancedDataID = -1;

    ShaderAsset shader;

  public:
    NineSliceRenderer();

    void add(const aseprite::Sprite &, const i16vec3 &position, const ivec2 &size, uint16 frame);

    void render(const Camera &);

};


#endif
