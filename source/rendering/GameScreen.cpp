
#include <game/dibidab.h>
#include "GameScreen.h"
#include "../game/Game.h"

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
    if (lvlScreen)
        lvlScreen->render(deltaTime);

    if (!Game::uiScreenManager->noScreens())
        Game::uiScreenManager->getActiveScreen().render(deltaTime);
}

void GameScreen::onResize()
{
    if (lvlScreen)
        lvlScreen->onResize();
}
