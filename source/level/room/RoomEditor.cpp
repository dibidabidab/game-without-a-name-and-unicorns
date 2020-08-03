#include "RoomEditor.h"


void RoomEditor::update(OrthographicCamera &cam, TileMap *room, DebugLineRenderer &lineRenderer)
{
    ImGui::ShowDemoWindow();

    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

    ImGui::Begin("Room editor");


    ivec2 hoveredTile(cam.cursorTo2DWorldPos() / vec2(TileMap::PIXELS_PER_TILE));
    if (MouseInput::pressed(GLFW_MOUSE_BUTTON_LEFT))
        room->setTile(hoveredTile.x, hoveredTile.y, placing);

    DebugTileRenderer::renderTile(lineRenderer, placing, hoveredTile.x, hoveredTile.y,
                                  room->contains(hoveredTile.x, hoveredTile.y) ? mu::Y : mu::X);

    for (auto t : TileMap::TILE_TYPES)
        if (KeyInput::justPressed(GLFW_KEY_0 + int(t)))
            placing = t;
    if (KeyInput::justPressed(GLFW_KEY_LEFT))
    {
        if (placing == Tile(0))
            placing = Tile(TileMap::TILE_TYPES.size() - 1);
        else
            placing = Tile((int(placing) - 1) % TileMap::TILE_TYPES.size());
    }
    if (KeyInput::justPressed(GLFW_KEY_RIGHT))
        placing = Tile((int(placing) + 1) % TileMap::TILE_TYPES.size());

    ImGui::End();
}
