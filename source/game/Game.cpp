
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

static SaveGame *saveGame = NULL;

SaveGame *Game::tryGetSaveGame()
{
    return saveGame;
}

SaveGame &Game::getSaveGame()
{
    if (saveGame == NULL)
        throw gu_err("There's currently no SaveGame active!");
    return *saveGame;
}

static std::string saveGamePath;

void Game::loadOrCreateSaveGame(const char *path)
{
    if (saveGame)
        throw gu_err("Please call unloadSaveGame() first. (and probably saveSaveGame() before that)");

    saveGamePath = path;
    saveGame = new SaveGame();

    if (File::exists(path))
    {
        json jsonData = json::from_cbor(File::readBinary(path));
        from_json(jsonData, *saveGame);
    }
}

void Game::saveSaveGame(const char *path)
{
    getSaveGame(); // can throw error :)

    std::vector<uint8> data;
    json::to_cbor(*saveGame, data);
    File::writeBinary(path == NULL ? saveGamePath.c_str() : path, data);
}

void Game::unloadSaveGame()
{
    delete saveGame;
    saveGame = NULL;
}

MegaSpriteSheet Game::spriteSheet;
cofu<Palettes3D> Game::palettes;
