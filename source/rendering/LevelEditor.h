
#ifndef GAME_LEVELEDITOR_H
#define GAME_LEVELEDITOR_H


#include "../level/Level.h"

class LevelEditor
{

    Level *lvl;
    int showingRoomProperties = -1;

  public:

    bool show = false;

    LevelEditor(Level *);

    void render();

};


#endif