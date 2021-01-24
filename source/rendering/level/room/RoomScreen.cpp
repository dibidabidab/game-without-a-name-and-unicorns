
#include <utils/code_editor/CodeEditor.h>
#include <level/Level.h>
#include <game/dibidab.h>
#include "RoomScreen.h"
#include "../../../generated/LimbJoint.hpp"
#include "../../../generated/Aiming.hpp"
#include "../../../generated/Leg.hpp"
#include "../../../ecs/components/component_methods.h"
#include "../../../generated/Physics.hpp"
#include "../../../generated/Light.hpp"
#include "../../../generated/VerletRope.hpp"
#include "../../../generated/PolyPlatform.hpp"
#include "../../../generated/PaletteSetter.hpp"
#include "../../../generated/Polyline.hpp"
#include "RoomEditor.h"
#include "../../../game/Game.h"

RoomScreen::RoomScreen(TiledRoom *room, bool showRoomEditor)
        :
        room(room), showRoomEditor(showRoomEditor),
        cam(.1, 1000, 0, 0),
        camMovement(room, &cam),
        applyPaletteShader(
                "applyPaletteShader", "shaders/fullscreen_quad.vert", "shaders/apply_palette.frag"
        ),
        postProcessingShader(
                "postProcessingShader", "shaders/fullscreen_quad.vert", "shaders/post_processing.frag"
        ),
        horizontalGaussianShader(
                "postProcessingShader", "shaders/fullscreen_quad.vert", "shaders/horizontal_gaussian_blur.frag"
        ),
        depthBufferDebugShader(
                "depthBufferDebugShader", "shaders/fullscreen_quad.vert", "shaders/debug_depth_buffer.frag"
        ),
        shadowCaster(room),
        lightMapRenderer(room),
        inspector(*room, "Room"),
        tileMap(&room->getMap())
{
    assert(room != NULL);
    assert(Game::spriteSheet.texture != NULL);
    Game::spriteSheet.printUsage();

    cam.position = mu::Z;
    cam.lookAt(mu::ZERO_3);

    inspector.createEntity_showSubFolder = "level_room";

    room->luaEnvironment["positionToScreenPosition"] = [&] (const ivec2 &pos) -> ivec2 {
        return cam.projectPixels(vec3(pos, 0));
    };
}

void RoomScreen::render(double deltaTime)
{
    if (tileMap != &room->getMap() || !tileMapRenderer) // recreate some things when the TileMap has been recreated/resized.
    {
        tileMap = &room->getMap();

        delete tileMapRenderer;
        tileMapRenderer = new TileMapRenderer(*room);
        delete bloodSplatterRenderer;
        bloodSplatterRenderer = new BloodSplatterRenderer(room);
    }

    camMovement.update(deltaTime);
    room->cursorPosition = cam.getCursorRayDirection() + cam.position;

    {
        gu::profiler::Zone z("lights/shadows");
        shadowCaster.updateShadowTexture(tileMapRenderer->fbo.colorTexture,
                                         !room->getMap().updatesPrevUpdate().empty());

        lightMapRenderer.render(cam, shadowCaster.fbo.colorTexture);

    }
    {
        gu::profiler::Zone z("fire particles");
        fireRenderer.renderParticles(room->entities, cam);
    }
    {   // render indexed stuff:
        gu::profiler::Zone z("indexed image");

        tileMapRenderer->updateMapTextureIfNeeded();
        bloodSplatterRenderer->updateSplatterTexture(deltaTime);

        indexedFbo->bind();

        uint zero = 0;
        glClearBufferuiv(GL_COLOR, 0, &zero);

        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        tileMapRenderer->render(cam, bloodSplatterRenderer->fbo.colorTexture);
        spriteRenderer.renderUsingAABBs(cam, room->entities);
        bloodSplatterRenderer->render(cam);
        polylineRenderer.render(room->entities, cam);
        fluidRenderer.render(room->entities, cam);
        fireRenderer.renderCombined();

        indexedFbo->unbind();

//        glDisable(GL_DEPTH_TEST);
    }
    {
        gu::profiler::Zone z("3d models");

//        glEnable(GL_DEPTH_TEST);

        hacky3dModelFbo->bind();

        modelRenderer.render(room->entities, cam);

        hacky3dModelFbo->unbind();

        glDisable(GL_DEPTH_TEST);
    }
    if (Game::settings.graphics.waterReflections)
    {
        gu::profiler::Zone z("reflections");

        fluidRenderer.renderReflections(indexedFbo, cam, room->getLevel().getTime());
    }
    {   // indexed image + lights + reflections --> low res RGB image + bloom image

        gu::profiler::Zone z("apply palette");

        rgbAndBloomFbo->bind();

        setPaletteEffect(deltaTime);
        applyPaletteShader.use();

        glUniform2i(applyPaletteShader.location("realResolution"), gu::widthPixels, gu::heightPixels);

        auto palettesTexture = Game::palettes->get3DTexture();
        palettesTexture->bind(0);
        glUniform1i(applyPaletteShader.location("palettes"), 0);
        glUniform1ui(applyPaletteShader.location("paletteEffect"), currentPaletteEffect);
        glUniform1ui(applyPaletteShader.location("prevPaletteEffect"), prevPaletteEffect);
        glUniform1f(applyPaletteShader.location("timeSinceNewPaletteEffect"), timeSinceNewPaletteEffect);

        fluidRenderer.reflectionsFbo->colorTexture->bind(1, applyPaletteShader, "reflectionsMap");
        lightMapRenderer.fbo->colorTexture->bind(2, applyPaletteShader, "lightMap");
        indexedFbo->colorTexture->bind(3, applyPaletteShader, "indexedImage");

        Mesh::getQuad()->render();

        rgbAndBloomFbo->unbind();
    }
    {
        if (Game::settings.graphics.bloom)
        {
            horizontalBlurFbo->bind();
            horizontalGaussianShader.use();

            rgbAndBloomFbo->colorTextures[1]->bind(0, postProcessingShader, "rgbImage");
            Mesh::getQuad()->render();

            horizontalBlurFbo->unbind();
        }

        postProcessingShader.use();

        rgbAndBloomFbo->colorTexture->bind(0, postProcessingShader, "rgbImage");
        horizontalBlurFbo->colorTexture->bind(1, postProcessingShader, "bloomImage");

        Mesh::getQuad()->render();
    }
    renderDebugStuff();
}

void RoomScreen::setPaletteEffect(float deltaTime)
{
    timeSinceNewPaletteEffect += deltaTime;
    int highestPriority = -99999;
    room->entities.view<PaletteSetter>().each([&](const PaletteSetter &p) {
        if (p.priority > highestPriority)
        {
            highestPriority = p.priority;
            int index = Game::palettes->effectIndex(p.paletteName);
            if (index != currentPaletteEffect && index != -1)
            {
                prevPaletteEffect = currentPaletteEffect;
                currentPaletteEffect = index;
                timeSinceNewPaletteEffect = 0;
            }
        }
    });
}

static int PIXELS_PER_PIXEL = 3;

void RoomScreen::onResize()
{
    cam.viewportWidth = ceil(gu::widthPixels / double(PIXELS_PER_PIXEL));
    cam.viewportHeight = ceil(gu::heightPixels / double(PIXELS_PER_PIXEL));
    cam.update();

    // create a new framebuffer to render the pixelated scene to:
    delete indexedFbo;
    indexedFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
    indexedFbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
    indexedFbo->addDepthTexture(GL_NEAREST, GL_NEAREST);

    delete rgbAndBloomFbo;
    rgbAndBloomFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
    rgbAndBloomFbo->addColorTexture(GL_RGB, GL_NEAREST, GL_NEAREST);    // pixel rgb values
    rgbAndBloomFbo->addColorTexture(GL_RGB, GL_LINEAR, GL_LINEAR);      // pixel bloom rgb values

    delete horizontalBlurFbo;
    horizontalBlurFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
    horizontalBlurFbo->addColorTexture(GL_RGB, GL_LINEAR, GL_LINEAR);

    fluidRenderer.onResize(cam);
    lightMapRenderer.onResize(cam);
    fireRenderer.onResize(cam);

    delete hacky3dModelFbo;
    hacky3dModelFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight);
    hacky3dModelFbo->bind();
    hacky3dModelFbo->colorTextures.push_back(indexedFbo->colorTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, indexedFbo->colorTexture->id, 0);
    hacky3dModelFbo->colorTextures.push_back(lightMapRenderer.fbo->colorTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightMapRenderer.fbo->colorTexture->id, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, indexedFbo->depthTexture->id, 0);
    hacky3dModelFbo->setDrawBuffers();
    hacky3dModelFbo->unbind();

}

void RoomScreen::renderDebugStuff()
{
    if (!dibidab::settings.showDeveloperOptions)
        return;

    gu::profiler::Zone z("debug");
    lineRenderer.projection = cam.combined;

    inspector.drawGUI(&cam, lineRenderer);

    static RoomEditor roomEditor;

    static bool
            renderDepthBuffer = false,
            renderTiles = false,
            renderWindArrows = false,
            debugLights = false,
            renderHitboxes = false,
            debugLegs = renderHitboxes,
            debugAimTargets = renderHitboxes;

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Room"))
    {
        ImGui::Separator();

        if (ImGui::MenuItem("Edit TileMap"))
            roomEditor.show = true;

        int ll = room->baseLightLevel;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Light level", &ll, 0, 2);
        ImGui::PopItemWidth();
        room->baseLightLevel = ll;

        ImGui::Separator();
        ImGui::MenuItem("Visualize stuff:", "", false, false);

        ImGui::MenuItem("show debug-tiles", "", &renderTiles);
        ImGui::MenuItem("show wind-arrows", "", &renderWindArrows);
        ImGui::MenuItem("show hitboxes & more", "", &renderHitboxes);
        ImGui::MenuItem("show depth-buffer", "", &renderDepthBuffer);
        ImGui::MenuItem("debug lights", "", &debugLights);
        ImGui::MenuItem("debug legs", "", &debugLegs);
        ImGui::MenuItem("debug aim targets", "", &debugAimTargets);

        ImGui::Separator();

        if (ImGui::MenuItem("View as JSON"))
        {
            auto &tab = CodeEditor::tabs.emplace_back();
            tab.title = room->name.empty() ? "Untitled room" : room->name;
            tab.title += " (READ-ONLY!)";

            json j;
            room->toJson(j);
            tab.code = j.dump(2);
            tab.languageDefinition = TextEditor::LanguageDefinition::C();
            tab.revert = [j] (auto &tab) {
                tab.code = j.dump(2);
            };
            tab.save = [] (auto &tab) {
                std::cerr << "Saving not supported" << std::endl;
            };
        }

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    if (renderDepthBuffer)
    {
        depthBufferDebugShader.use();
        indexedFbo->depthTexture->bind(0, depthBufferDebugShader, "depthTexture");
        Mesh::getQuad()->render();

        if (!ImGui::IsAnyWindowHovered())
            ImGui::SetTooltip("Z-index: %.2f",
                  indexedFbo->getPixelDepth(MouseInput::mouseX / PIXELS_PER_PIXEL, indexedFbo->height - MouseInput::mouseY / PIXELS_PER_PIXEL)
                  * -(cam.far_ - cam.near_) + cam.position.z - cam.near_
            );
    }

    {
        lineRenderer.scale = TileMap::PIXELS_PER_TILE;
        if (renderTiles)
        {
            renderDebugBackground();
            renderDebugTiles();
        }
        roomEditor.update(cam, *room, lineRenderer);

        if (renderWindArrows)
            this->renderWindArrows();

        lineRenderer.scale = 1;
    }

    if (renderHitboxes)
    {
        room->entities.view<AABB>().each([&](auto e, AABB &body) {

            auto p = room->entities.try_get<Physics>(e);
            if (p)
                draw(*p, body, lineRenderer, mu::X);
            else
                draw(body, lineRenderer, mu::X);
        });
        room->entities.view<VerletRope>().each([&](VerletRope &rope) {
            for (int i = 1; i < rope.points.size(); i++)
                lineRenderer.line(rope.points.at(i - 1).currentPos, rope.points.at(i).currentPos, vec3(mu::X));
        });

        auto drawPolyLine = [&](const AABB &aabb, const Polyline &line, auto &) {

            if (line.points.empty())
                return;

            auto it = line.points.begin();
            for (int i = 0; i < line.points.size() - 1; i++)
            {
                vec2 p0 = *it + vec2(aabb.center);
                vec2 p1 = *(++it) + vec2(aabb.center);
                lineRenderer.line(p0, p1, mu::X + mu::Z);
                lineRenderer.circle(p0, 2, 8, vec3(1));
            }
        };
        room->entities.view<AABB, Polyline, PolyPlatform>().each(drawPolyLine);
        room->entities.view<AABB, Polyline, Fluid>().each(drawPolyLine);
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

    if (debugLights)
    {
        static vec3 lightColor(1, 1, 0);
        room->entities.view<AABB, PointLight>().each([&](AABB &aabb, PointLight &light) {

            lineRenderer.circle(aabb.center, 2, 10, lightColor);
            for (float angle = 0; angle < 360; angle += 36)
            {
                vec2 dir = rotate(vec2(6, 0), angle * mu::DEGREES_TO_RAD);
                lineRenderer.line(vec2(aabb.center) + dir * .5f, vec2(aabb.center) + dir, lightColor);
            }
        });
        room->entities.view<AABB, DirectionalLight>().each([&](AABB &aabb, DirectionalLight &light) {

            float rads = light.rotation * mu::DEGREES_TO_RAD;

            vec2 dir = rotate(vec2(light.width * .5, 0), rads);
            lineRenderer.line(vec2(aabb.center) - dir, vec2(aabb.center) + dir, lightColor);
            lineRenderer.arrow(aabb.center, vec2(aabb.center) + rotate(vec2(0, -10), rads), 3, lightColor);
        });
        shadowCaster.drawDebugLines(cam);
    }
}

void RoomScreen::renderDebugBackground()
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

void RoomScreen::renderDebugTiles()
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

void RoomScreen::renderWindArrows()
{
    auto &map = room->getMap();
    auto &wind = room->getWindMap();
    for (int x = 0; x < map.width; x++)
        for (int y = 0; y < map.height; y++)
        {
            auto &dir = wind.get(x, y);
            if (dir.x == 0 && dir.y == 0)
                continue;
            lineRenderer.arrow(vec2(x, y), vec2(x, y) + dir, .1, vec3(1));
        }
}

RoomScreen::~RoomScreen()
{
    delete indexedFbo;
    delete rgbAndBloomFbo;
    delete horizontalBlurFbo;
    delete hacky3dModelFbo;
}
