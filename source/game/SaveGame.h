
#ifndef GAME_SAVEGAME_H
#define GAME_SAVEGAME_H

#include <json.hpp>
#include "../luau.h"

struct SaveGame
{
    SaveGame();

    sol::table luaTable;
};

void to_json(json &, const SaveGame &);
void from_json(const json &, SaveGame &);

#endif
