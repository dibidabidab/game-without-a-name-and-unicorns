
#ifndef GAME_LEVELEDITOR_H
#define GAME_LEVELEDITOR_H


#include "../level/Level.h"

class LevelEditor
{

    Level *lvl;

  public:

    LevelEditor(Level *);

    void render();

};


#endif