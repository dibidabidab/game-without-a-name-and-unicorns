
#ifndef GAME_SHADOWCASTER_H
#define GAME_SHADOWCASTER_H

#include <graphics/frame_buffer.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/shader_asset.h>
#include <graphics/camera.h>
#include "../../../level/room/Room.h"
#include "../../../level/room/ecs/components/graphics/Light.h"
#include "../../../level/room/ecs/components/physics/Physics.h"

class ShadowCaster
{
    Room *room;

    SharedMesh shadowMesh;

    ShaderAsset debugShader, shader;

  public:

    const static int
            TEXTURE_SIZE = 2048, SIZE_PER_LIGHT = 256,

            MAX_SHADOWS_PER_LIGHT = 128, VERTS_PER_SHADOW = 9, INDICES_PER_SHADOW = 24;

    FrameBuffer fbo;

    ShadowCaster(Room *room);

    void updateMesh(const PointLight &light, const vec2 &lightPos);

    void drawDebugLines(const Camera &cam);

    void updateShadowTexture(const SharedTexture &tileMapTexture, bool tileMapChanged);

};


#endif
