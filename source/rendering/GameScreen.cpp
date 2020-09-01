
#include "GameScreen.h"

GameScreen::GameScreen()
{
    onSessionChange = Game::onSessionChange += [&] {
        delete lvlScreen;
        lvlScreen = NULL;

        Session *session = Game::tryGetCurrentSession();
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
