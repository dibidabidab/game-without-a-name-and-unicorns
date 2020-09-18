
#include "LevelScreen.h"
#include "MiniMapTextureGenerator.h"
#include "../../generated/PlayerControlled.hpp"
#include "../../generated/TransRoomable.hpp"

LevelScreen::LevelScreen(Level *lvl) : lvl(lvl), lvlEditor(lvl)
{
    onPlayerEnteredRoom = lvl->onPlayerEnteredRoom += [this](Room *room, int playerId)
    {
        auto localPlayer = Game::getCurrentSession().getLocalPlayer();
        if (!localPlayer || localPlayer->id != playerId)
            return;
        std::cout << "Local player entered room. Show RoomScreen\n";
        showRoom(room);
    };

    onRoomDeletion = lvl->beforeRoomDeletion += [this](Room *r)
    {
        if (roomScreen && r == roomScreen->room)
        {
            delete roomScreen;
            roomScreen = NULL;
        }
    };
}

void LevelScreen::render(double deltaTime)
{
    glClearColor(32 / 255., 53 / 255., 189 / 255., 1);
    glClear(GL_COLOR_BUFFER_BIT);

    renderDebugTools();
    if (roomScreen)
        roomScreen->render(deltaTime);
}

void LevelScreen::onResize()
{
    if (roomScreen)
        roomScreen->onResize();
}

void LevelScreen::renderDebugTools()
{
    if (!Game::settings.showDeveloperOptions)
        return;

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Level"))
    {
        auto str = std::to_string(lvl->getNrOfRooms()) + " room(s) active.";
        ImGui::MenuItem(str.c_str(), NULL, false, false);

        if (ImGui::MenuItem("Level editor"))
            lvlEditor.show = true;

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    lvlEditor.render();
    if (lvlEditor.moveCameraToRoom >= 0)
        showRoom(&lvl->getRoom(lvlEditor.moveCameraToRoom));
}

void LevelScreen::showRoom(Room *r)
{
    delete roomScreen;
    roomScreen = NULL;
    if (r)
    {
        roomScreen = new RoomScreen(r, true);
        roomScreen->onResize();
        if (Game::settings.graphics.roomTransitionAnimation)
            r->entities.view<LocalPlayer, TransRoomed>().each([&](auto, TransRoomed &t) {
                roomScreen->camMovement.offsetAnim = normalize(vec2(t.travelDir)) * -10.f;
            });
    }
}

