
#include <game/dibidab.h>
#include "GameScreen.h"
#include "../game/Game.h"
#include "PaletteEditor.h"

GameScreen::GameScreen()
{
    onSessionChange = dibidab::onSessionChange += [&] {
        delete lvlScreen;
        lvlScreen = NULL;

        Session *session = dibidab::tryGetCurrentSession();
        if (session)
            onNewLevel = session->onNewLevel += [&] (Level *lvl) {
                lvlScreen = new LevelScreen(lvl);
            };
    };
}

void GameScreen::render(double deltaTime)
{
    static int prevScaling = Game::settings.graphics.pixelScaling;
    if (Game::settings.graphics.pixelScaling != prevScaling)
    {
        prevScaling = Game::settings.graphics.pixelScaling;
        gu::onResize();
        onResize();
    }

    if (lvlScreen)
        lvlScreen->render(deltaTime);

    if (!Game::uiScreenManager->noScreens())
        Game::uiScreenManager->getActiveScreen().render(deltaTime);

    if (!dibidab::settings.showDeveloperOptions)
        return;

    static PaletteEditor pE;
    pE.drawGUI(RoomScreen::currentPaletteEffect);

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Game"))
    {
        if (ImGui::BeginMenu("Graphics"))
        {
            if (ImGui::MenuItem("Palette Editor"))
                pE.show = true;
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void GameScreen::onResize()
{
    if (lvlScreen)
        lvlScreen->onResize();
}
