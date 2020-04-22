
#ifndef GAME_ROOMSCREEN_H
#define GAME_ROOMSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <utils/quad_renderer.h>
#include <imgui.h>

#include <utils/aseprite/AsepriteLoader.h>
#include "../../level/room/Room.h"
#include "../../level/room/RoomEditor.h"
#include "DebugTileRenderer.h"
#include "../../level/ecs/components/Physics.h"
#include "../../level/ecs/components/PlatformerMovement.h"
#include "../../level/ecs/components/Networked.h"
#include "../../macro_magic/component.h"
#include "../../level/ecs/EntityInspector.h"
#include "CameraMovement.h"
#include "tile_map/TileMapRenderer.h"

class RoomScreen : public Screen
{
    bool showRoomEditor = false;

    Room *room;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;
    CameraMovement camMovement;

    FrameBuffer *indexedFbo = nullptr;

    TileMapRenderer tileMapRenderer;

    ShaderProgram applyPaletteShader;

  public:

    RoomScreen(Room *room, bool showRoomEditor=false)
        : room(room), showRoomEditor(showRoomEditor),
        cam(.1, 100, 0, 0),
        camMovement(room, &cam),
        tileMapRenderer(&room->getMap()),
        applyPaletteShader(ShaderProgram::fromFiles(
            "applyPaletteShader", "assets/shaders/apply_palette.vert", "assets/shaders/apply_palette.frag"
        ))
    {
        assert(room != NULL);

        cam.position = mu::Z;
        cam.lookAt(mu::ZERO_3);

        tileMapRenderer.tileSets.insert({TileMaterial::brick, TileSet("assets/sprites/tileset_bricks.ase")});

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void render(double deltaTime) override
    {
        #ifndef EMSCRIPTEN
        ShaderProgram::reloadFromFile = int(room->getLevel()->getTime() * 2) % 2 == 0;
        #endif

        camMovement.update(deltaTime);

        tileMapRenderer.updateMapTextureIfNeeded();

        glClearColor(.5, 0, .1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        {   // render indexed stuff:

            indexedFbo->bind();

            uint zero = 0;
            glClearBufferuiv(GL_COLOR, 0, &zero);

            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            tileMapRenderer.render(cam);

            glDisable(GL_DEPTH_TEST);
            indexedFbo->unbind();
        }
        {   // indexed image --> RGB image

            applyPaletteShader.use();
            indexedFbo->colorTexture->bind(0, applyPaletteShader, "indexedImage");
            Mesh::getQuad()->render();
        }
        renderDebugStuff();
    }

    void onResize() override
    {
        cam.viewportWidth = gu::widthPixels / 3;
        cam.viewportHeight = gu::heightPixels / 3;
        cam.update();

        // create a new framebuffer to render the pixelated scene to:
        delete indexedFbo;
        indexedFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
        indexedFbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
        indexedFbo->addDepthTexture(GL_NEAREST, GL_NEAREST);
    }

    void renderDebugStuff()
    {
        lineRenderer.projection = cam.combined;

        lineRenderer.scale = TileMap::PIXELS_PER_TILE;
        renderDebugBackground();
        renderDebugTiles();

        if (showRoomEditor)
        {
            static RoomEditor roomEditor;
            static bool editRoom = false;
            if (!editRoom) editRoom = ImGui::Button("edit tileMap");
            else {
                editRoom = !ImGui::Button("stop editing tileMap");
                roomEditor.update(cam, &room->getMap(), lineRenderer);
            }
        }
        lineRenderer.scale = 1;

        room->entities.view<Physics, AABB>().each([&](auto e, Physics &p, AABB &body) {
            p.draw(body, lineRenderer, mu::Z);
        });
        lineRenderer.axes(mu::ZERO_3, 16, vec3(1));

        EntityInspector(&room->entities).drawGUI(&cam, lineRenderer);
    }

    void renderDebugBackground()
    {
        TileMap &map = room->getMap();

        // grid
        for (int x = 0; x < map.width; x++)
            lineRenderer.line(vec2(x, 0), vec2(x, map.height), vec3(.3));
        for (int y = 0; y < map.height; y++)
            lineRenderer.line(vec2(0, y), vec2(map.width, y), vec3(.3));

        // x-axis
        lineRenderer.line(
                vec3(0), vec2(map.width, 0), mu::X
        );
        // y-axis
        lineRenderer.line(
                vec3(0), vec2(0, map.height), mu::Y
        );
    }

    void renderDebugTiles()
    {
        TileMap &map = room->getMap();
        auto color = vec3(1);
        // all tiles:
        for (int x = 0; x < map.width; x++)
            for (int y = 0; y < map.height; y++)
                DebugTileRenderer::renderTile(lineRenderer, map.getTile(x, y), x, y, color);
        // tile outlines:
        for (auto &outline : map.getOutlines())
            lineRenderer.line(outline.first, outline.second, mu::Z + mu::X);
    }
};


#endif
