
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
#include "../../level/ecs/components/physics/Physics.h"
#include "../../level/ecs/components/PlatformerMovement.h"
#include "../../level/ecs/components/Networked.h"
#include "../../macro_magic/component.h"
#include "../../level/ecs/EntityInspector.h"
#include "CameraMovement.h"
#include "tile_map/TileMapRenderer.h"
#include "../PaletteEditor.h"
#include "../../level/ecs/components/graphics/Light.h"
#include "light/ShadowCaster.h"
#include "light/LightMapRenderer.h"
#include "../../level/ecs/components/physics/VerletRope.h"
#include "../sprites/MegaSpriteSheet.h"
#include "../sprites/SpriteRenderer.h"
#include "../PolylineRenderer.h"
#include "../../level/ecs/components/body_parts/Leg.h"
#include "../../level/ecs/components/body_parts/LimbJoint.h"
#include "../../level/ecs/components/combat/Aiming.h"
#include "../../level/ecs/components/Polyline.h"
#include "../../level/ecs/components/physics/PolyPlatform.h"

class RoomScreen : public Screen
{
    bool showRoomEditor = false;

    Room *room;

    EntityInspector inspector;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;
    CameraMovement camMovement;

    FrameBuffer *indexedFbo = nullptr;

    TileMapRenderer tileMapRenderer;

    ShaderAsset applyPaletteShader;
    Palettes3D palettes;
    PaletteEditor paletteEditor;

    ShadowCaster shadowCaster;
    LightMapRenderer lightMapRenderer;

    const MegaSpriteSheet *spriteSheet;
    SpriteRenderer spriteRenderer;

    PolylineRenderer polylineRenderer;

  public:

    RoomScreen(Room *room, const MegaSpriteSheet *spriteSheet, bool showRoomEditor=false)
        :
        room(room), showRoomEditor(showRoomEditor),
        cam(.1, 100, 0, 0),
        camMovement(room, &cam),
        tileMapRenderer(&room->getMap()),
        applyPaletteShader(
            "applyPaletteShader", "shaders/apply_palette.vert", "shaders/apply_palette.frag"
        ),
        shadowCaster(room),
        lightMapRenderer(room),
        inspector(room->entities),
        spriteSheet(spriteSheet),
        spriteRenderer(spriteSheet)
    {
        assert(room != NULL);
        assert(spriteSheet != NULL);
        assert(spriteSheet->texture != NULL);
        spriteSheet->printUsage();

        cam.position = mu::Z;
        cam.lookAt(mu::ZERO_3);

        tileMapRenderer.tileSets.insert({TileMaterial::brick, asset<TileSet>("sprites/bricks")});

        // Temporary creation of test entities
        entt::entity lamp1 = room->getTemplate("Lamp").create();
        room->getChildComponentByName<AABB>(lamp1, "rope").center = ivec2(130, 125);

        entt::entity lamp2 = room->getTemplate("Lamp").create();
        room->getChildComponentByName<AABB>(lamp2, "rope").center = ivec2(220, 125);

//        for (int x=85; x < 180; x+=mu::randomInt(4, 15)) {
//            entt::entity plant = room->getTemplate("Plant").create();
//            room->getChildComponentByName<AABB>(plant, "rope").center = ivec2(x, 16);
//        }

        entt::entity enemy = room->getTemplate("Enemy").create();
        room->entities.get<AABB>(enemy).center = ivec2(400, 125);

        room->getTemplate("Rainbow").create();
    }

    void render(double deltaTime) override
    {
        camMovement.update(deltaTime);
        room->cursorPositionInRoom = cam.getCursorRayDirection() + cam.position;

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

            tileMapRenderer.render(cam);
            spriteRenderer.render(deltaTime, cam, room->entities);
            polylineRenderer.render(room->entities, cam);

            glDisable(GL_DEPTH_TEST);
            indexedFbo->unbind();
        }
        {
            gu::profiler::Zone z("lights & shadows");
            shadowCaster.updateShadowTexture(tileMapRenderer.fbo.colorTexture, !room->getMap().updatesPrevUpdate().empty());

            lightMapRenderer.render(cam, shadowCaster.fbo.colorTexture);
        }
        {   // indexed image + lights --> RGB image

            gu::profiler::Zone z("apply palette");

            applyPaletteShader.use();

            glUniform2i(applyPaletteShader.location("realResolution"), gu::widthPixels, gu::heightPixels);

            auto palettesTexture = palettes.get3DTexture();
            palettesTexture->bind(0);
            glUniform1i(applyPaletteShader.location("palettes"), 0);

            indexedFbo->colorTexture->bind(1, applyPaletteShader, "indexedImage");
            lightMapRenderer.fbo->colorTexture->bind(2, applyPaletteShader, "lightMap");

            Mesh::getQuad()->render();
        }
        renderDebugStuff();
    }

    void onResize() override
    {
        cam.viewportWidth = ceil(gu::widthPixels / 3.);
        cam.viewportHeight = ceil(gu::heightPixels / 3.);
        cam.update();

        // create a new framebuffer to render the pixelated scene to:
        delete indexedFbo;
        indexedFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
        indexedFbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
        indexedFbo->addDepthBuffer();
    }

    void renderDebugStuff()
    {
        gu::profiler::Zone z("debug");

        lineRenderer.projection = cam.combined;
        lineRenderer.scale = TileMap::PIXELS_PER_TILE;

        ImGui::SetNextWindowPos(ImVec2(530, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(180, 250), ImGuiCond_FirstUseEver);

        static bool
            vsync = false,
            renderTiles = false,
            renderShadowDebugLines = false,
            renderHitboxes = false,
            debugLegs = renderHitboxes,
            debugAimTargets = renderHitboxes,
            debugPolyPlatforms = renderHitboxes;

        if (ImGui::Begin("debug tools"))
        {
            ImGui::Checkbox("vsync", &vsync);
            ImGui::Checkbox("render debug-tiles", &renderTiles);
            ImGui::Checkbox("render shadow-debug-lines", &renderShadowDebugLines);
            ImGui::Checkbox("show hitboxes & more", &renderHitboxes);
            ImGui::Checkbox("debug legs", &debugLegs);
            ImGui::Checkbox("debug aim targets", &debugAimTargets);
            ImGui::Checkbox("debug poly-platforms", &debugPolyPlatforms);
            inspector.show |= ImGui::Button("entity inspector");
            paletteEditor.show |= ImGui::Button("palette editor");

            gu::setVSync(vsync);

            if (showRoomEditor)
            {
                static bool editRoom = false;
                if (!editRoom) editRoom = ImGui::Button("edit tileMap");
                else {
                    static RoomEditor roomEditor;
                    editRoom = !ImGui::Button("stop editing tileMap");
                    roomEditor.update(cam, &room->getMap(), lineRenderer);
                }
            }
        }
        ImGui::End();
        if (renderTiles)
        {
            renderDebugBackground();
            renderDebugTiles();
        }

        lineRenderer.scale = 1;

        if (renderHitboxes)
        {
            room->entities.view<AABB>().each([&](auto e, AABB &body) {

                auto p = room->entities.try_get<Physics>(e);
                if (p)
                    p->draw(body, lineRenderer, mu::X);
                else
                    body.draw(lineRenderer, mu::X);
            });
            room->entities.view<VerletRope>().each([&](VerletRope &rope) {
                for (int i = 1; i < rope.points.size(); i++)
                    lineRenderer.line(rope.points.at(i - 1).currentPos, rope.points.at(i).currentPos, vec3(mu::X));
            });
            lineRenderer.axes(mu::ZERO_3, 16, vec3(1));
        }

        if (debugLegs)
        {
            room->entities.view<Leg, AABB>().each([&](Leg &leg, AABB &aabb) {
                lineRenderer.axes(leg.target, 4, leg.moving ? mu::Y : (leg.shouldBeMoving ? mu::Y + mu::X : mu::X));

                auto body = room->entities.try_get<AABB>(leg.body);

                if (body)
                    lineRenderer.axes(body->center + leg.anchor, 2, mu::Z);
            });

            room->entities.view<LimbJoint, AABB>().each([&](const LimbJoint &knee, AABB &aabb) {

                AABB
                    *hip = room->entities.try_get<AABB>(knee.hipJointEntity),
                    *foot = room->entities.try_get<AABB>(knee.footEntity);

                Leg *leg = room->entities.try_get<Leg>(knee.footEntity);

                if (!hip || !foot || !leg)
                    return;

                lineRenderer.line(hip->center, aabb.center, mu::X + mu::Z);
                lineRenderer.line(foot->center, aabb.center, mu::X + mu::Z);
            });
        }
        if (debugAimTargets) room->entities.view<Aiming, AABB>().each([&](const Aiming &aim, const AABB &aabb) {
            lineRenderer.axes(aim.target, 4, mu::X);
            lineRenderer.line(aim.target, aabb.center, mu::X);
        });
        if (debugPolyPlatforms) room->entities.view<AABB, Polyline, PolyPlatform>().each([&](const AABB &aabb, const Polyline &line, auto &) {

            auto it = line.points.begin();
            for (int i = 0; i < line.points.size() - 1; i++)
            {
                vec2 p0 = *it + vec2(aabb.center);
                vec2 p1 = *(++it) + vec2(aabb.center);
                lineRenderer.line(p0, p1, mu::X + mu::Z);
                lineRenderer.circle(p0, 2, 8, vec3(1));
            }
        });

        inspector.entityTemplates = room->getTemplateNames();
        inspector.drawGUI(&cam, lineRenderer);
        if (!inspector.templateToCreate.empty())
            room->getTemplate(inspector.templateToCreate).create();

        paletteEditor.drawGUI(palettes);

        if (renderShadowDebugLines)
            shadowCaster.drawDebugLines(cam);
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
        for (int x = 0; x < map.width; x++)
            for (int y = 0; y < map.height; y++)
                DebugTileRenderer::renderTile(lineRenderer, map.getTile(x, y), x, y, color);
        // tile outlines:
        for (auto &outline : map.getOutlines())
            lineRenderer.line(outline.first, outline.second, mu::Z + mu::X);
    }

    ~RoomScreen()
    {
        delete indexedFbo;
    }
};


#endif
