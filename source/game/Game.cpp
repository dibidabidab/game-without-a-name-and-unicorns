
#include <game/dibidab.h>
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
        saveSettings();
        return;
    }
    try {
        json j = json::parse(File::readString(SETTINGS_FILE_PATH));
        settings = j;
        dibidab::settings = j;
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
    j.merge_patch(dibidab::settings);

    File::writeBinary(SETTINGS_FILE_PATH, j.dump(4));
}

MegaSpriteSheet Game::spriteSheet;
cofu<Palettes3D> Game::palettes;

cofu<UIScreenManager> Game::uiScreenManager;
