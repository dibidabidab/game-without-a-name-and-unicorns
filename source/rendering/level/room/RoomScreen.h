
#ifndef GAME_ROOMSCREEN_H
#define GAME_ROOMSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <graphics/shader_asset.h>

#include "../../../tiled_room/TiledRoom.h"
#include "CameraMovement.h"
#include "tile_map/TileMapRenderer.h"
#include "light/ShadowCaster.h"
#include "light/LightMapRenderer.h"
#include "../../sprites/SpriteRenderer.h"
#include "../../PolylineRenderer.h"
#include "blood_splatter/BloodSplatterRenderer.h"
#include "FluidRenderer.h"

class RoomScreen : public Screen
{
    bool showRoomEditor = false;

//    EntityInspector inspector;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;

    FrameBuffer
        *indexedFbo = NULL,
        *rgbAndBloomFbo = NULL,
        *horizontalBlurFbo = NULL;

    TileMapRenderer *tileMapRenderer = NULL;
    BloodSplatterRenderer *bloodSplatterRenderer = NULL;

    ShaderAsset applyPaletteShader, horizontalGaussianShader, postProcessingShader;

    ShadowCaster shadowCaster;
    LightMapRenderer lightMapRenderer;

    SpriteRenderer spriteRenderer;

    PolylineRenderer polylineRenderer;
    FluidRenderer fluidRenderer;

    TileMap *tileMap = NULL;

  public:

    static inline uint currentPaletteEffect = 0, prevPaletteEffect = 0;
    static inline float timeSinceNewPaletteEffect = 0;
    CameraMovement camMovement;

    TiledRoom *room;

    RoomScreen(TiledRoom *room, bool showRoomEditor=false);

    void render(double deltaTime) override;

    void setPaletteEffect(float deltaTime);

    void onResize() override;

    void renderDebugStuff();

    void renderDebugBackground();

    void renderDebugTiles();

    void renderWindArrows();

    ~RoomScreen();
};


#endif
