
#ifndef GAME_LEVELSCREEN_H
#define GAME_LEVELSCREEN_H

#include <gu/screen.h>
#include <level/Level.h>
#include "room/RoomScreen.h"
#include "LevelEditor.h"

class LevelScreen : public Screen
{

    Level *lvl;

    RoomScreen *roomScreen = NULL;

    LevelEditor lvlEditor;

    delegate_method onPlayerEnteredRoom, onRoomDeletion;

  public:

    LevelScreen(Level *);

    void render(double deltaTime) override;

    void onResize() override;

    void showRoom(TiledRoom *);

  private:

    void renderDebugTools();

};


#endif
