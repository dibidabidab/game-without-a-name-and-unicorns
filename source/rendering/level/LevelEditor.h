
#ifndef GAME_LEVELEDITOR_H
#define GAME_LEVELEDITOR_H


#include <level/Level.h>
#include <graphics/texture.h>

class LevelEditor
{

    Level *lvl;
    int showingRoomProperties = -1;

    SharedTexture miniMapTex;

  public:

    int moveCameraToRoom = -1;

    bool show = false;

    LevelEditor(Level *);

    void render();

  private:

    void moveButtons();

    void roomProperties();

    void miniMap();

};


#endif