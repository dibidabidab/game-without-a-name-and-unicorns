
#ifndef GAME_ROOMSCREEN_H
#define GAME_ROOMSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <utils/quad_renderer.h>
#include <imgui.h>

#include "level/room/Room.h"
#include "level/room/RoomEditor.h"
#include "level/room/DebugTileRenderer.h"
#include "level/ecs/components/Physics.h"
#include "level/ecs/components/PlatformerMovement.h"
#include "level/ecs/components/Networked.h"
#include "macro_magic/component.h"
#include "level/ecs/EntityInspector.h"

class RoomScreen : public Screen
{
    Room *room;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;

    FrameBuffer *fbo = nullptr;

  public:

    RoomScreen(Room *room) : room(room), cam(.1, 100, 0, 0)
    {
        assert(room != NULL);

        cam.position = mu::Z;
        cam.lookAt(mu::ZERO_3);
    }

    void render(double deltaTime) override
    {
        fbo->bind();
        glClearColor(.5, 0, .1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        TileMap &map= room->getMap();

        lineRenderer.projection = cam.combined;

        lineRenderer.scale = TileMap::PIXELS_PER_TILE;
        renderDebugBackground();

        renderDebugTiles();

        static RoomEditor roomEditor;
        static bool editRoom = false;
//        if (!editRoom) editRoom = ImGui::Button("edit tileMap");
//        else {
//            editRoom = !ImGui::Button("stop editing tileMap");
//            roomEditor.update(cam, tileMap, lineRenderer);
//        }
        lineRenderer.scale = 1;

        room->entities.view<Physics, AABB>().each([&](auto e, Physics &p, AABB &body) {
            p.draw(body, lineRenderer, mu::Z);
        });

        fbo->unbind();

        QuadRenderer::render(fbo->colorTexture);

        lineRenderer.axes(mu::ZERO_3, 16, vec3(1));

        EntityInspector(&room->entities).drawGUI(&cam, lineRenderer);
    }

    void onResize() override
    {
        cam.viewportWidth = int(gu::widthPixels / 2);
        cam.viewportHeight = int(gu::heightPixels / 2);
        cam.update();

        // create a new framebuffer to render the pixelated scene to:
        delete fbo;
        fbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
        fbo->addColorTexture(GL_RGB, GL_NEAREST, GL_NEAREST);
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
