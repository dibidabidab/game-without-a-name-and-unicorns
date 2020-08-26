
#ifndef GAME_LEVELSCREEN_H
#define GAME_LEVELSCREEN_H

#include <gu/screen.h>
#include "../level/Level.h"
#include "room/RoomScreen.h"
#include "LevelEditor.h"

class LevelScreen : public Screen
{

    Level *lvl;
    int localPlayerID;

    RoomScreen *roomScreen = NULL;

    LevelEditor lvlEditor;

  public:

    LevelScreen(Level *, int localPlayerID);

    void render(double deltaTime) override;

    void onResize() override;

  private:

    void renderDebugTools();

};


#endif
