
#include <game/dibidab.h>
#include "Game.h"

Game::Settings Game::settings;

#define SETTINGS_FILE_PATH "./settings.json"

void setShaderDefinitions()
{
    if (Game::settings.graphics.vignette)
        ShaderDefinitions::global().define("VIGNETTE");
    else
        ShaderDefinitions::global().undef("VIGNETTE");

    if (Game::settings.graphics.bloom)
        ShaderDefinitions::global().define("BLOOM");
    else
        ShaderDefinitions::global().undef("BLOOM");

    if (Game::settings.graphics.waterReflections)
        ShaderDefinitions::global().define("WATER_REFLECTIONS");
    else
        ShaderDefinitions::global().undef("WATER_REFLECTIONS");


    if (Game::settings.graphics.fireHeatDistortion)
        ShaderDefinitions::global().define("FIRE_HEAT_DISTORTION");
    else
        ShaderDefinitions::global().undef("FIRE_HEAT_DISTORTION");
}

void Game::loadSettings()
{
    if (!File::exists(SETTINGS_FILE_PATH))
    {
        settings = Settings();
        std::cerr << "Settings file (" << SETTINGS_FILE_PATH << ") not found!" << std::endl;
        saveSettings();
        loadSettings();// hack for loading font
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
    json j = dibidab::settings;
    j.merge_patch(settings);

    File::writeBinary(SETTINGS_FILE_PATH, j.dump(4));
}

MegaSpriteSheet Game::spriteSheet;
cofu<Palettes3D> Game::palettes;

cofu<UIScreenManager> Game::uiScreenManager;
