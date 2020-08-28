
#include <utils/code_editor/CodeEditor.h>
#include "RoomScreen.h"

RoomScreen::RoomScreen(Room *room, bool showRoomEditor)
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
        shadowCaster(room),
        lightMapRenderer(room),
        inspector(room->entities),
        spriteRenderer(&Game::spriteSheet),
        tileMap(&room->getMap())
{
    assert(room != NULL);
    assert(Game::spriteSheet.texture != NULL);
    Game::spriteSheet.printUsage();

    cam.position = mu::Z;
    cam.lookAt(mu::ZERO_3);
}

void RoomScreen::render(double deltaTime)
{
    if (tileMap != &room->getMap() || !tileMapRenderer) // recreate some things when the TileMap has been recreated/resized.
    {
        tileMap = &room->getMap();

        delete tileMapRenderer;
        tileMapRenderer = new TileMapRenderer(tileMap);
        delete bloodSplatterRenderer;
        bloodSplatterRenderer = new BloodSplatterRenderer(room);
    }

    camMovement.update(deltaTime);
    room->cursorPositionInRoom = cam.getCursorRayDirection() + cam.position;

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
        spriteRenderer.render(deltaTime, cam, room->entities);
        bloodSplatterRenderer->render(cam);
        polylineRenderer.render(room->entities, cam);
        fluidRenderer.render(room->entities, cam);

        glDisable(GL_DEPTH_TEST);
        indexedFbo->unbind();
    }
    {
        gu::profiler::Zone z("lights/shadows/reflections");
        shadowCaster.updateShadowTexture(tileMapRenderer->fbo.colorTexture, !room->getMap().updatesPrevUpdate().empty());

        lightMapRenderer.render(cam, shadowCaster.fbo.colorTexture);

        if (Game::settings.graphics.waterReflections)
            fluidRenderer.renderReflections(indexedFbo, cam, room->getLevel().getTime());
    }
    {   // indexed image + lights + reflections --> low res RGB image + bloom image

        gu::profiler::Zone z("apply palette");

        rgbAndBloomFbo->bind();

        setPaletteEffect(deltaTime);
        applyPaletteShader.use();

        glUniform2i(applyPaletteShader.location("realResolution"), gu::widthPixels, gu::heightPixels);

        auto palettesTexture = palettes.get3DTexture();
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
            int index = palettes.effectIndex(p.paletteName);
            if (index != currentPaletteEffect && index != -1)
            {
                prevPaletteEffect = currentPaletteEffect;
                currentPaletteEffect = index;
                timeSinceNewPaletteEffect = 0;
            }
        }
    });
}

void RoomScreen::onResize()
{
    cam.viewportWidth = ceil(gu::widthPixels / 3.);
    cam.viewportHeight = ceil(gu::heightPixels / 3.);
    cam.update();

    // create a new framebuffer to render the pixelated scene to:
    delete indexedFbo;
    indexedFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
    indexedFbo->addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
    indexedFbo->addDepthTexture(GL_NEAREST, GL_NEAREST);

    delete rgbAndBloomFbo;
    rgbAndBloomFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
    rgbAndBloomFbo->addColorTexture(GL_RGB, GL_NEAREST, GL_NEAREST);    // pixel rgb values
    rgbAndBloomFbo->addColorTexture(GL_RGB, GL_LINEAR, GL_LINEAR);      // pixel bloom rgb values

    delete horizontalBlurFbo;
    horizontalBlurFbo = new FrameBuffer(cam.viewportWidth, cam.viewportHeight, 0);
    horizontalBlurFbo->addColorTexture(GL_RGB, GL_LINEAR, GL_LINEAR);

    fluidRenderer.onResize(cam);
}

void RoomScreen::renderDebugStuff()
{
    if (!Game::settings.showDeveloperOptions)
        return;

    gu::profiler::Zone z("debug");
    lineRenderer.projection = cam.combined;

    {
        inspector.entityTemplates = room->getTemplateNames();
        inspector.drawGUI(&cam, lineRenderer);
        if (!inspector.templateToCreate.empty())
            room->getTemplate(inspector.templateToCreate).create();
        if (!inspector.templateToEdit.empty())
            editLuaTemplateCode();
    }
    static RoomEditor roomEditor;

    static bool
            renderTiles = false,
            renderWindArrows = false,
            debugLights = false,
            renderHitboxes = false,
            debugLegs = renderHitboxes,
            debugAimTargets = renderHitboxes;

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Graphics"))
    {
        paletteEditor.show |= ImGui::MenuItem("Palette editor");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Room"))
    {
        if (ImGui::BeginMenu("Systems"))
        {
            for (auto sys : room->getSystems())
                ImGui::MenuItem(sys->name.c_str(), NULL, &sys->enabled);

            ImGui::EndMenu();
        }

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
        ImGui::MenuItem("debug lights", "", &debugLights);
        ImGui::MenuItem("debug legs", "", &debugLegs);
        ImGui::MenuItem("debug aim targets", "", &debugAimTargets);
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    {
        lineRenderer.scale = TileMap::PIXELS_PER_TILE;
        if (renderTiles)
        {
            renderDebugBackground();
            renderDebugTiles();
        }
        roomEditor.update(cam, &room->getMap(), lineRenderer);

        if (renderWindArrows)
            this->renderWindArrows();

        lineRenderer.scale = 1;
    }

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

    paletteEditor.drawGUI(palettes, currentPaletteEffect);

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

void RoomScreen::editLuaTemplateCode()
{
    auto &entityTemplate = room->getTemplate(inspector.templateToEdit);
    if (!dynamic_cast<LuaEntityTemplate *>(&entityTemplate))
        return;

    auto script = ((LuaEntityTemplate *) &entityTemplate)->script;

    for (auto &t : CodeEditor::tabs)
        if (t.title == script.getLoadedAsset().fullPath)
            return;

    auto &tab = CodeEditor::tabs.emplace_back();
    tab.title = script.getLoadedAsset().fullPath;
    tab.code = script->source;
    tab.languageDefinition = TextEditor::LanguageDefinition::C(); // the lua definition is pretty broken
    tab.save = [script] (auto &tab) {

        File::writeBinary(script.getLoadedAsset().fullPath.c_str(), tab.code); // todo: why is this called writeBINARY? lol

        AssetManager::loadFile(script.getLoadedAsset().fullPath, "assets/");
    };
    tab.revert = [script] (auto &tab) {
        tab.code = script->source;
    };
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
    TileMap &map = room->getMap();
    auto &wind = map.wind;
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
}
