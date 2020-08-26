
#include "LevelEditor.h"
#include "../level/ecs/components/PlayerControlled.h"
#include "MiniMapTextureGenerator.h"
#include <imgui.h>

LevelEditor::LevelEditor(Level *lvl) : lvl(lvl)
{

}

void LevelEditor::render()
{
    moveCameraToRoom = -1;

    if (!show)
    {
        miniMapTex = NULL;
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGui::SetNextWindowContentSize(ImVec2(8000, 8000));

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_HorizontalScrollbar;

    if (!ImGui::Begin("Level editor", &show, window_flags))
    {
        miniMapTex = NULL;
        ImGui::End();
        return;
    }

    static bool firstTime = true;
    static float zoom = 4;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Editor"))
        {
            static bool importing = false;
            static std::vector<std::string> levelPaths;

            if (ImGui::BeginMenu("Import Room from other Level"))
            {
                if (!importing)
                {
                    levelPaths.clear();
                    importing = true;

                    File::iterateDirectoryRecursively(".", [&](const std::string &path, bool isDir) {
                        if (isDir || !stringEndsWith(path, ".lvl"))
                            return;
                        levelPaths.push_back(path);
                    });
                }

                for (auto &path : levelPaths)
                    if (ImGui::MenuItem(path.c_str()))
                    {}

                ImGui::EndMenu();
            }
            else importing = false;

            if (ImGui::BeginMenu("Create Room"))
            {
                static int width = 24, height = 16;

                ImGui::SliderInt("Width", &width, 1, 255);
                ImGui::SliderInt("Height", &height, 1, 255);

                if (ImGui::Button("Create"))
                {
                    lvl->createRoom(width, height);
                    showingRoomProperties = lvl->getNrOfRooms() - 1;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Move all rooms"))
            {
                ivec2 move(0);

                ImGui::PushButtonRepeat(true);

                if (ImGui::ArrowButton("right", ImGuiDir_Right))
                    move.x++;
                ImGui::SameLine(0, 10);
                if (ImGui::ArrowButton("up", ImGuiDir_Up))
                    move.y++;
                ImGui::SameLine(0, 10);
                if (ImGui::ArrowButton("left", ImGuiDir_Left))
                    move.x--;
                ImGui::SameLine(0, 10);
                if (ImGui::ArrowButton("down", ImGuiDir_Down))
                    move.y--;

                ImGui::PopButtonRepeat();

                bool canMove = true;

                for (int i = 0; i < lvl->getNrOfRooms(); i++)
                    if (-move.x > int(lvl->getRoom(i).positionInLevel.x) || -move.y > int(lvl->getRoom(i).positionInLevel.y))
                        canMove = false;

                if (canMove && (move.x != 0 || move.y != 0))
                {
                    for (int i = 0; i < lvl->getNrOfRooms(); i++)
                        lvl->getRoom(i).positionInLevel += move;
                    miniMapTex = NULL;
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();

            ImGui::SliderFloat("Zoom", &zoom, 1.f, 10.f);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (firstTime)
        ImGui::SetScrollY(ImGui::GetScrollMaxY());

    constexpr float DIST = 32;

    auto lowerLeft = ImGui::GetWindowPos();
    lowerLeft.y += ImGui::GetWindowSize().y - DIST;
    lowerLeft.y += ImGui::GetScrollMaxY() - ImGui::GetScrollY();
    lowerLeft.x -= ImGui::GetScrollX() - DIST;


    auto drawList = ImGui::GetWindowDrawList();

    auto mousePos = ImGui::GetMousePos();

    // resize room values:
    static int moveLeftBorder = 0, moveRightBorder = 0, moveTopBorder = 0, moveBottomBorder = 0;
    #define resetRoomResizing() moveLeftBorder = moveRightBorder = moveTopBorder = moveBottomBorder = 0

    static bool recreateMiniMap = false;

    if (!miniMapTex || recreateMiniMap)
    {
        miniMapTex = MiniMapTextureGenerator::generate(*lvl);
        recreateMiniMap = false;
    }

    auto imgPtr = (void*)(intptr_t)miniMapTex->id;
    drawList->AddImage(imgPtr, ImVec2(lowerLeft.x, lowerLeft.y - miniMapTex->height * zoom), ImVec2(lowerLeft.x + miniMapTex->width * zoom, lowerLeft.y));

    for (int i = 0; i < lvl->getNrOfRooms(); i++)
    {
        Room &room = lvl->getRoom(i);

        ImVec2 p0(lowerLeft.x + room.positionInLevel.x * zoom, lowerLeft.y - (room.positionInLevel.y + room.getMap().height) * zoom);
        ImVec2 p1(p0.x + room.getMap().width * zoom, lowerLeft.y - room.positionInLevel.y * zoom);

        auto col = ImVec4(1, 1, 1, .7);

        if (p0.x < mousePos.x && p1.x > mousePos.x && p0.y < mousePos.y && p1.y > mousePos.y)
        {
            col = ImVec4(.3, .5, 1, 1);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
            {
                showingRoomProperties = i;
                resetRoomResizing();
            }
        }
        if (i == showingRoomProperties)
        {
            col = ImVec4(1, 1, 1, 1);

            drawList->AddRect(
                ImVec2(p0.x - moveLeftBorder * zoom, p0.y - moveTopBorder * zoom),
                ImVec2(p1.x + moveRightBorder * zoom, p1.y + moveBottomBorder * zoom),
                ImGui::GetColorU32(ImVec4(1, 1, 1, sin(glfwGetTime() * 5.) * .5 + .5))
            );
        }

        drawList->AddRect(p0, p1, ImGui::GetColorU32(col), zoom, ImDrawCornerFlags_All, zoom);

        if (!room.entities.view<LocalPlayer>().empty())
            drawList->AddText(p0, IM_COL32_WHITE, "\n   LocalPlayer");
    }

    ImGui::End();

    if (showingRoomProperties >= lvl->getNrOfRooms())
        showingRoomProperties = -1;

    if (showingRoomProperties >= 0)
    {
        Room &room = lvl->getRoom(showingRoomProperties);

        ImGui::SetNextWindowPos(ImVec2(50, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);

        bool open = true;
        ImGui::Begin("Room", &open);
        if (!open)
            showingRoomProperties = -1;

        ImGui::Text("Index: %d", showingRoomProperties);
        ImGui::Text("Size: (%d, %d)", room.getMap().width, room.getMap().height);

        if (ImGui::Button("Move camera here"))
            moveCameraToRoom = showingRoomProperties;

        if (ImGui::Button("Duplicate Room"))
        {
            showingRoomProperties = lvl->getNrOfRooms();
            lvl->createRoom(room.getMap().width, room.getMap().height, &room);
            recreateMiniMap = true;
        }

        if (ImGui::Button("Delete Room"))
            ImGui::OpenPopup("Deleting Room");

        if (ImGui::BeginPopupModal("Deleting Room", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure?\nThere's no such thing as CTRL+Z here");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                lvl->deleteRoom(showingRoomProperties);
                ImGui::CloseCurrentPopup();
                recreateMiniMap = true;
                showingRoomProperties = -1;
            }

            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Text("Position:");
        ivec2 pos = room.positionInLevel;
        if (ImGui::InputInt("x", &pos.x, 1, 16))
            recreateMiniMap = true;
        if (ImGui::InputInt("y", &pos.y, 1, 16))
            recreateMiniMap = true;

        room.positionInLevel.x = max(0, pos.x);
        room.positionInLevel.y = max(0, pos.y);

        ImGui::Separator();
        ImGui::Text("Resize:");

        ImGui::InputInt("Move right border", &moveRightBorder, 1, 10);
        ImGui::InputInt("Move top border", &moveTopBorder, 1, 10);
        ImGui::InputInt("Move left border", &moveLeftBorder, 1, 10);
        ImGui::InputInt("Move bottom border", &moveBottomBorder, 1, 10);

        if (ImGui::Button("Resize"))
        {
            room.resize(moveLeftBorder, moveRightBorder, moveTopBorder, moveBottomBorder);
            resetRoomResizing();
            recreateMiniMap = true;
        }

        ImGui::End();
    }

    firstTime = false;
}
