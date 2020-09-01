
#include "Game.h"

Game::Settings Game::settings;

#define SETTINGS_FILE_PATH "./settings.json"

void setShaderDefinitions()
{
    if (Game::settings.graphics.vignette)
        ShaderDefinitions::define("VIGNETTE");
    else
        ShaderDefinitions::undef("VIGNETTE");
    if (Game::settings.graphics.bloom)
        ShaderDefinitions::define("BLOOM");
    else
        ShaderDefinitions::undef("BLOOM");
    if (Game::settings.graphics.waterReflections)
        ShaderDefinitions::define("WATER_REFLECTIONS");
    else
        ShaderDefinitions::undef("WATER_REFLECTIONS");
}

void Game::loadSettings()
{
    if (!File::exists(SETTINGS_FILE_PATH))
    {
        settings = Settings();
        std::cerr << "Settings file (" << SETTINGS_FILE_PATH << ") not found!" << std::endl;
        return;
    }
    try {
        settings = json::parse(File::readString(SETTINGS_FILE_PATH));
    }
    catch (std::exception &e)
    {
        settings = Settings();
        std::cerr << "Error while loading settings:\n" << e.what() << std::endl;
    }
    setShaderDefinitions();
}

void Game::saveSettings()
{
    setShaderDefinitions();
    json j = settings;

    File::writeBinary(SETTINGS_FILE_PATH, j.dump(4));
}

std::map<std::string, std::string> Game::startupArgs;

Session &Game::getCurrentSession()
{
    auto *s = tryGetCurrentSession();
    if (s == NULL)
        throw gu_err("There's no Session active at the moment");
    return *s;
}

delegate<void()> Game::onSessionChange;
Session *currSession = NULL;

Session *Game::tryGetCurrentSession()
{
    return currSession;
}

void Game::setCurrentSession(Session *s)
{
    delete currSession;
    currSession = s;
    onSessionChange();
}

MegaSpriteSheet Game::spriteSheet;
cofu<Palettes3D> Game::palettes;

cofu<UIScreenManager> Game::uiScreenManager;
