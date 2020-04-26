
#ifndef GAME_ROOMSCREEN_H
#define GAME_ROOMSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <graphics/shader_asset.h>
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
#include "../PaletteEditor.h"
#include "../../level/ecs/components/Light.h"

class RoomScreen : public Screen
{
    bool showRoomEditor = false;

    Room *room;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;
    CameraMovement camMovement;

    FrameBuffer *indexedFbo = nullptr;

    TileMapRenderer tileMapRenderer;

    ShaderAsset applyPaletteShader;
    Palettes3D palettes;
    PaletteEditor paletteEditor;

  public:

    RoomScreen(Room *room, bool showRoomEditor=false)
        : room(room), showRoomEditor(showRoomEditor),
        cam(.1, 100, 0, 0),
        camMovement(room, &cam),
        tileMapRenderer(&room->getMap()),
        applyPaletteShader(
            "applyPaletteShader", "shaders/apply_palette.vert", "shaders/apply_palette.frag"
        )
    {
        assert(room != NULL);

        cam.position = mu::Z;
        cam.lookAt(mu::ZERO_3);

        tileMapRenderer.tileSets.insert({TileMaterial::brick, asset<TileSet>("sprites/bricks")});

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void render(double deltaTime) override
    {
        camMovement.update(deltaTime);

        glClearColor(32 / 255., 53 / 255., 189 / 255., 1);
        glClear(GL_COLOR_BUFFER_BIT);

        {   // render indexed stuff:
            gu::profiler::Zone z("indexed image");

            tileMapRenderer.updateMapTextureIfNeeded();

            indexedFbo->bind();

            uint zero = 0;
            glClearBufferuiv(GL_COLOR, 0, &zero);

            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

//            tileMapRenderer.render(cam);

            glDisable(GL_DEPTH_TEST);
            indexedFbo->unbind();
        }
        {   // indexed image --> RGB image

            gu::profiler::Zone z("apply palette");

            applyPaletteShader.use();

            auto palettesTexture = palettes.get3DTexture();
            palettesTexture->bind(0);
            glUniform1i(applyPaletteShader.location("palettes"), 0);

            indexedFbo->colorTexture->bind(1, applyPaletteShader, "indexedImage");
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
        gu::profiler::Zone z("debug");

        lineRenderer.projection = cam.combined;

        lineRenderer.scale = TileMap::PIXELS_PER_TILE;
//        renderDebugBackground();
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

        room->entities.view<AABB>().each([&](auto e, AABB &body) {

            auto p = room->entities.try_get<Physics>(e);
            if (p)
                p->draw(body, lineRenderer, mu::X);
            else
                body.draw(lineRenderer, mu::X);

            auto l = room->entities.try_get<LightPoint>(e);
            if (l)
                lineRenderer.axes(body.center, l->radius, vec3(1, 1, 0));
        });
        lineRenderer.axes(mu::ZERO_3, 16, vec3(1));

        EntityInspector inspector(&room->entities);
        inspector.entityTemplates = room->getTemplateNames();
        inspector.drawGUI(&cam, lineRenderer);
        if (!inspector.templateToCreate.empty())
            room->getTemplate(inspector.templateToCreate)->create();

        paletteEditor.drawGUI(palettes);

        const TileMapOutlines &outlines = room->getMap().getOutlines();

        room->entities.view<AABB, LightPoint>().each([&](AABB &aabb, LightPoint &light) {
            if (!light.castShadow)
                return;
            gu::profiler::Zone z("shadow casting");

            for (auto &outline : outlines)
            {
                const vec2 line[2] = {outline.first * vec2(16), outline.second * vec2(16)};
                const vec2 &lightPos = aabb.center;

                bool render = false;
                float distance = length(line[0] - lightPos);
                if (distance <= light.radius)
                    render = true;
                else
                {
                    distance = length(line[1] - lightPos);
                    render = distance <= light.radius;
                }

                if (!render)
                    continue;

                vec2 lineNormal = line[1] - line[0];
                lineNormal = normalize(vec2(-lineNormal.y, lineNormal.x));

                vec2 diffToLine = ((line[0] + line[1]) * vec2(.5)) - lightPos;

                float angle = acos(dot(normalize(diffToLine), lineNormal)) * mu::RAD_TO_DEGREES;
                if (angle < 90)
                    continue;

                // line normal:
                lineRenderer.line((line[0] + line[1]) * vec2(.5), (line[0] + line[1]) * vec2(.5) + lineNormal * vec2(16), vec3(1));

                // actual line:
                lineRenderer.line(line[0], line[1], mu::X);

                vec2 avgDir(0);
                vec2 castEnd[2];
                for (int i = 0; i < 2; i++)
                {
                    vec2 diff = line[i] - lightPos;
                    vec2 dir = normalize(diff);
                    avgDir += dir;
                    castEnd[i] = line[i] + dir * vec2(light.radius);
                    lineRenderer.line(line[i], castEnd[i], mu::X);
                }
                avgDir = normalize(avgDir);

                vec2 diffToCastLine = ((castEnd[0] + castEnd[1]) * vec2(.5)) - lightPos;

                float castDepth = length(diffToLine - diffToCastLine);
                float additionalDepth = light.radius - castDepth;

                lineRenderer.line(castEnd[0], castEnd[0] + avgDir * vec2(additionalDepth), mu::Y);
                lineRenderer.line(castEnd[1], castEnd[1] + avgDir * vec2(additionalDepth), mu::Y);

                lineRenderer.line(castEnd[1] + avgDir * vec2(additionalDepth), castEnd[0] + avgDir * vec2(additionalDepth), mu::Y);
                lineRenderer.line(castEnd[1] + avgDir * vec2(additionalDepth), castEnd[0] + avgDir * vec2(additionalDepth), mu::Y);
            }


        });
    }

    void renderDebugBackground()
    {
        gu::profiler::Zone z("background grid");
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
        gu::profiler::Zone z("tiles grid+outlines");
        TileMap &map = room->getMap();
        auto color = vec3(1);
        // all tiles:
//        for (int x = 0; x < map.width; x++)
//            for (int y = 0; y < map.height; y++)
//                DebugTileRenderer::renderTile(lineRenderer, map.getTile(x, y), x, y, color);
        // tile outlines:
//        for (auto &outline : map.getOutlines())
//            lineRenderer.line(outline.first, outline.second, mu::Z + mu::X);
    }
};


#endif
