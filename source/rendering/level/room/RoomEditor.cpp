#include "RoomEditor.h"
#include "../../../game/Game.h"
#include "../../../tiled_room/TiledRoom.h"


void RoomEditor::update(OrthographicCamera &cam, TiledRoom &room, DebugLineRenderer &lineRenderer)
{
    if (!show)
        return;

    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 670), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Room editor", &show, ImGuiWindowFlags_NoNav))
    {
        ImGui::End();
        return;
    }

    auto *selectedLayerPtr = &room.getMap();
    {
        ImGui::Text("LAYER:");
        ImGui::BeginChild("Layer", ImVec2(0, 100), true);
        {
            if (ImGui::Selectable((room.getMap().name + " (Collision layer)").c_str(), selectedLayer == 0))
                selectedLayer = 0;
            ImGui::SameLine(180);
            ImGui::TextDisabled("z: %.2f%%", room.getMap().zIndex);
            int i = 0;
            for (auto &l : room.decorativeTileLayers)
            {
                if (ImGui::Selectable((l.name + "##" + std::to_string(i)).c_str(), selectedLayer == ++i))
                    selectedLayer = i;
                if (selectedLayer == i)
                    selectedLayerPtr = &l;

                ImGui::SameLine(180);
                ImGui::TextDisabled("z: %.2f%%", l.zIndex);
            }
        }
        ImGui::EndChild();

        if (ImGui::Button("New"))
        {
            room.decorativeTileLayers.emplace_back(ivec2(room.getMap().width, room.getMap().height));
            room.decorativeTileLayers.back().name = "new layer";
            selectedLayer = room.decorativeTileLayers.size();
            selectedLayerPtr = &room.decorativeTileLayers.back();
        }
        if (selectedLayer > 0)
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                auto it = room.decorativeTileLayers.begin();
                for (int i = 1; i < selectedLayer; i++) it++;
                room.decorativeTileLayers.erase(it);
                while (selectedLayer > room.decorativeTileLayers.size())
                    selectedLayer--;
                selectedLayerPtr = selectedLayer == 0 ? &room.getMap() : &(*std::next(room.decorativeTileLayers.begin(), selectedLayer - 1));
            }
        }

        ImGui::NewLine();

        const int bufferLen = 128;
        char *ptr = new char[bufferLen]();
        memcpy(ptr, &selectedLayerPtr->name[0], min<int>(bufferLen, selectedLayerPtr->name.length()));
        if (ImGui::InputText("Name", ptr, bufferLen))
            selectedLayerPtr->name = std::string(ptr);
        delete[] ptr;

        ImGui::SliderFloat("Z-index", &selectedLayerPtr->zIndex, 0, 100);//cam.position.z + -cam.far_, cam.position.z - cam.near_);
    }

    auto &map = *selectedLayerPtr;

    /////////////

    static int brushSize = 1;

    constexpr int MOUSE_PRIORITY = 1;
    MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, MOUSE_PRIORITY, 5);
    MouseInput::capture(GLFW_MOUSE_BUTTON_RIGHT, MOUSE_PRIORITY, 5);

    ivec2 hoveredTile(cam.cursorTo2DWorldPos() / vec2(TileMap::PIXELS_PER_TILE));

    int startX = hoveredTile.x - (brushSize / 2);
    int startY = hoveredTile.y - (brushSize / 2);
    for (int x = startX; x < startX + brushSize; x++)
    {
        for (int y = startY; y < startY + brushSize; y++)
        {
            if (MouseInput::pressed(GLFW_MOUSE_BUTTON_LEFT, MOUSE_PRIORITY))
                map.setTile(x, y, placing, material);
            if (MouseInput::pressed(GLFW_MOUSE_BUTTON_RIGHT, MOUSE_PRIORITY))
                map.setTile(x, y, map.getTile(x, y), material);

            if (map.contains(x, y))
                DebugTileRenderer::renderTile(lineRenderer, placing, x, y,
                                              map.getMaterialProperties(material).color);
        }
    }

    {
        ImGui::NewLine();
        ImGui::Separator();

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
            if (++material == map.nrOfMaterialTypes)
                material = 0;
        if (KeyInput::justPressed(Game::settings.keyInput.prevTileMaterial))
            if (material-- == 0)
                material = map.nrOfMaterialTypes - 1;

        int radMat = material;
        for (TileMaterial m = 0; m < map.nrOfMaterialTypes; m++)
            ImGui::RadioButton(map.getMaterialProperties(m).name.c_str(), &radMat, m);
        material = radMat;
    }
    {
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::DragInt("Brush size", &brushSize, 1, 1, 24);
    }

    ImGui::End();
}
