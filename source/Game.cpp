
#include "Game.h"

Game::Settings Game::settings;

#define SETTINGS_FILE_PATH "./settings.json"

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
}

void Game::saveSettings()
{
    json j = settings;

    File::writeBinary(SETTINGS_FILE_PATH, j.dump(4));
}

MegaSpriteSheet Game::spriteSheet;
cofu<Palettes3D> Game::palettes;
