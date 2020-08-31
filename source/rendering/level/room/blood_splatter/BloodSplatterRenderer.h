
#ifndef GAME_BLOODSPLATTERRENDERER_H
#define GAME_BLOODSPLATTERRENDERER_H

#include <graphics/frame_buffer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include <graphics/3d/mesh.h>

class Room;

class BloodSplatterRenderer
{

    Room *room;
    ShaderAsset shader;
    SharedMesh quad;
    VertData instancedData;
    int instancedDataID = -1;
    int nrOfDrops = 0;

  public:
    FrameBuffer fbo;

    BloodSplatterRenderer(Room *);

    void updateSplatterTexture(float deltaTime);

    void render(const Camera &);

};


#endif
