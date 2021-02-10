
#include "LevelScreen.h"
#include "MiniMapTextureGenerator.h"
#include <generated/PlayerControlled.hpp>
#include <game/dibidab.h>
#include <imgui.h>
#include "../../generated/TransRoomable.hpp"
#include "../../game/Game.h"

LevelScreen::LevelScreen(Level *lvl) : lvl(lvl), lvlEditor(lvl)
{
    onPlayerEnteredRoom = lvl->onPlayerEnteredRoom += [this](Room *room, int playerId)
    {
        auto localPlayer = dibidab::getCurrentSession().getLocalPlayer();
        if (!localPlayer || localPlayer->id != playerId)
            return;
        std::cout << "Local player entered room. Show RoomScreen\n";
        showRoom(dynamic_cast<TiledRoom *>(room));
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
    vec3 bgColor = Game::palettes->effects.front().lightLevels->get().colors.at(1).second; // todo: dirty
    glClearColor(bgColor.r, bgColor.g, bgColor.b, 1);
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
    if (!dibidab::settings.showDeveloperOptions)
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
        showRoom(dynamic_cast<TiledRoom *>(&lvl->getRoom(lvlEditor.moveCameraToRoom)));
}

void LevelScreen::showRoom(TiledRoom *r)
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

