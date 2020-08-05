#include "RoomEditor.h"
#include "../../Game.h"


void RoomEditor::update(OrthographicCamera &cam, TileMap *room, DebugLineRenderer &lineRenderer)
{
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

    ImGui::Begin("Room editor");

    ivec2 hoveredTile(cam.cursorTo2DWorldPos() / vec2(TileMap::PIXELS_PER_TILE));
    if (MouseInput::pressed(GLFW_MOUSE_BUTTON_LEFT))
        room->setTile(hoveredTile.x, hoveredTile.y, placing, material);
    if (MouseInput::pressed(GLFW_MOUSE_BUTTON_RIGHT))
        room->setTile(hoveredTile.x, hoveredTile.y, room->getTile(hoveredTile.x, hoveredTile.y), material);

    if (room->contains(hoveredTile.x, hoveredTile.y))
    {
        DebugTileRenderer::renderTile(lineRenderer, placing, hoveredTile.x, hoveredTile.y,
                                      room->getMaterialProperties(material).color);
    }

    {
        ImGui::Text("TILE SHAPE:");
        ImGui::BulletText("Press left-mouse-button to change\na Tile's shape.");
        ImGui::BulletText("Loop with [%s] and [%s]", KeyInput::getKeyName(Game::settings.keyInput.prevTileShape), KeyInput::getKeyName(Game::settings.keyInput.nextTileShape));

        asset<Texture> tilesImg("tiles");

        auto imgPtr = (void*)(intptr_t)tilesImg->id;

        float uvWidth = 1. / TileMap::TILE_TYPES.size();

        for (Tile t : TileMap::TILE_TYPES)
        {
            int tI = int(t);
            ImGui::PushID(tI);
            if (ImGui::ImageButton(
                imgPtr, ImVec2(32, 32),
                ImVec2(tI * uvWidth, 0), ImVec2((tI + 1) * uvWidth, 1),
                -1, ImVec4(0, 0, 0, 0),
                t == placing ? ImVec4(.5, .25, 1, 1) : ImVec4(1, 1, 1, 1)
            ))
                placing = t;
            ImGui::PopID();

            if (tI % 4 != 0)
                ImGui::SameLine();
        }
        ImGui::NewLine();

        for (auto t : TileMap::TILE_TYPES)
            if (KeyInput::justPressed(GLFW_KEY_0 + int(t)))
                placing = t;
        if (KeyInput::justPressed(Game::settings.keyInput.prevTileShape))
        {
            if (placing == Tile(0))
                placing = Tile(TileMap::TILE_TYPES.size() - 1);
            else
                placing = Tile((int(placing) - 1) % TileMap::TILE_TYPES.size());
        }
        if (KeyInput::justPressed(Game::settings.keyInput.nextTileShape))
            placing = Tile((int(placing) + 1) % TileMap::TILE_TYPES.size());
    }

    {
        ImGui::NewLine();
        ImGui::Separator();

        ImGui::Text("PAINT TILE MATERIAL:");
        ImGui::BulletText("Press right-mouse-button to change\na Tile's material.");
        ImGui::BulletText("Loop with [%s] and [%s]", KeyInput::getKeyName(Game::settings.keyInput.prevTileMaterial), KeyInput::getKeyName(Game::settings.keyInput.nextTileMaterial));

        if (KeyInput::justPressed(Game::settings.keyInput.nextTileMaterial))
            if (++material == room->nrOfMaterialTypes)
                material = 0;
        if (KeyInput::justPressed(Game::settings.keyInput.prevTileMaterial))
            if (material-- == 0)
                material = room->nrOfMaterialTypes - 1;

        int radMat = material;
        for (TileMaterial m = 0; m < room->nrOfMaterialTypes; m++)
            ImGui::RadioButton(room->getMaterialProperties(m).name.c_str(), &radMat, m);
        material = radMat;
    }

    ImGui::End();
}
