
#ifndef GAME_LEVELSCREEN_H
#define GAME_LEVELSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <utils/quad_renderer.h>

#include "level/Level.h"
#include "level/RoomEditor.h"
#include "level/DebugTileRenderer.h"
#include "level/ecs/components/Physics.h"

class LevelScreen : public Screen
{
    Level level;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;

    FrameBuffer *fbo = nullptr;

  public:

    LevelScreen() : cam(.1, 100, 0, 0)
    {
        cam.position = mu::Z;
        cam.lookAt(mu::ZERO_3);

        auto player = level.entities.create();
        level.entities.assign<Physics>(player, ivec2(5, 13), ivec2(32, 32));
    }

    void render(double deltaTime) override
    {
        level.update(deltaTime);

        fbo->bind();
        glClearColor(.5, 0, .1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        Room *room = level.getCurrentRoom();

        lineRenderer.projection = cam.combined;

        lineRenderer.scale = Level::PIXELS_PER_BLOCK;
        renderDebugBackground();

        renderDebugTiles();

        static RoomEditor roomEditor;
        roomEditor.update(cam, room, lineRenderer);
        lineRenderer.scale = 1;

        level.entities.view<Physics>().each([&](auto e, Physics &p) {
            p.body.draw(lineRenderer, mu::X);
        });

        fbo->unbind();

        QuadRenderer::render(fbo->colorTexture);
    }

    void onResize() override
    {
        cam.viewportWidth = int(gu::widthPixels / 3);
        cam.viewportHeight = int(gu::heightPixels / 3);
        cam.update();

        // create a new framebuffer to render the pixelated scene to:
        delete fbo;
        fbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
        fbo->addColorTexture(GL_RGB, GL_NEAREST, GL_NEAREST);
    }

    void renderDebugBackground()
    {
        Room *room = level.getCurrentRoom();

        // grid
        for (int x = 0; x < room->width; x++)
            lineRenderer.line(vec3(x, 0, 0), vec3(x, room->height, 0), vec3(.3));
        for (int y = 0; y < room->height; y++)
            lineRenderer.line(vec3(0, y, 0), vec3(room->width, y, 0), vec3(.3));

        // x-axis
        lineRenderer.line(
                vec3(0), vec3(room->width, 0, 0), mu::X
        );
        // y-axis
        lineRenderer.line(
                vec3(0), vec3(0, room->height, 0), mu::Y
        );
    }

    void renderDebugTiles()
    {
        Room *room = level.getCurrentRoom();
        auto color = vec3(1);
        // all tiles:
        for (int x = 0; x < room->width; x++)
            for (int y = 0; y < room->height; y++)
                DebugTileRenderer::renderTile(lineRenderer, room->getTile(x, y), x, y, color);
        // tile outlines:
        for (auto &outline : room->getOutlines())
            lineRenderer.line(vec3(outline.first, 0), vec3(outline.second, 0), mu::Z + mu::X);
    }
};


#endif
