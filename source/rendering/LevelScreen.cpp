
#include "LevelScreen.h"

LevelScreen::LevelScreen(Level *lvl, int localPlayerID) : lvl(lvl), localPlayerID(localPlayerID)
{
    lvl->onPlayerEnteredRoom = [this](Room *room, int playerId)
    {
        if (this->localPlayerID != playerId)
            return;
        delete roomScreen;
        std::cout << "Local player entered room. Show RoomScreen\n";
        roomScreen = new RoomScreen(room, true);
        roomScreen->onResize();
    };
}

void LevelScreen::render(double deltaTime)
{
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

        ImGui::MenuItem("Level editor");
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

