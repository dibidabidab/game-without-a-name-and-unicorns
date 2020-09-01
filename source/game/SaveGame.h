
#ifndef GAME_SAVEGAME_H
#define GAME_SAVEGAME_H

#include <json.hpp>
#include "../luau.h"

struct SaveGame
{
    SaveGame(const char *path);

    sol::table luaTable;

    void save(const char *path=NULL); // if path == NULL then same path from constructor is used.

  private:
    std::string loadedFromPath;
};

#endif
