
#ifndef GAME_LEVELSCREEN_H
#define GAME_LEVELSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <utils/quad_renderer.h>
#include <imgui.h>

#include "level/Level.h"
#include "level/RoomEditor.h"
#include "level/DebugTileRenderer.h"
#include "level/ecs/components/Physics.h"
#include "level/ecs/components/PlatformerMovement.h"
#include "level/ecs/components/Networked.h"
#include "macro_magic/component.h"

class LevelScreen : public Screen
{
    Level *level = NULL;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;

    FrameBuffer *fbo = nullptr;

    entt::entity player = level->entities.create();

  public:

    LevelScreen(Level *level) : level(level), cam(.1, 100, 0, 0)
    {
        assert(level != NULL);

        cam.position = mu::Z;
        cam.lookAt(mu::ZERO_3);

        level->entities.assign<Physics>(player, ivec2(5, 13), ivec2(32, 52));
        level->entities.assign<LocalPlayer>(player);
        level->entities.assign<PlatformerMovement>(player);
        level->entities.assign<Networked>(player,
            // to send:
            Networked::components<PlatformerMovement, Physics>(),
            // to receive:
            Networked::components<>()
        );
    }

    void render(double deltaTime) override
    {
        fbo->bind();
        glClearColor(.5, 0, .1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        Room *room = level->getCurrentRoom();

        lineRenderer.projection = cam.combined;

        lineRenderer.scale = Level::PIXELS_PER_BLOCK;
        renderDebugBackground();

        renderDebugTiles();

        static RoomEditor roomEditor;
        roomEditor.update(cam, room, lineRenderer);
        lineRenderer.scale = 1;

        level->entities.view<Physics>().each([&](auto e, Physics &p) {
            p.draw(lineRenderer, mu::Z);
        });

        fbo->unbind();

        QuadRenderer::render(fbo->colorTexture);
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
        Room *room = level->getCurrentRoom();

        // grid
        for (int x = 0; x < room->width; x++)
            lineRenderer.line(vec2(x, 0), vec2(x, room->height), vec3(.3));
        for (int y = 0; y < room->height; y++)
            lineRenderer.line(vec2(0, y), vec2(room->width, y), vec3(.3));

        // x-axis
        lineRenderer.line(
                vec3(0), vec2(room->width, 0), mu::X
        );
        // y-axis
        lineRenderer.line(
                vec3(0), vec2(0, room->height), mu::Y
        );
    }

    void renderDebugTiles()
    {
        Room *room = level->getCurrentRoom();
        auto color = vec3(1);
        // all tiles:
        for (int x = 0; x < room->width; x++)
            for (int y = 0; y < room->height; y++)
                DebugTileRenderer::renderTile(lineRenderer, room->getTile(x, y), x, y, color);
        // tile outlines:
        for (auto &outline : room->getOutlines())
            lineRenderer.line(outline.first, outline.second, mu::Z + mu::X);
    }
};


#endif
