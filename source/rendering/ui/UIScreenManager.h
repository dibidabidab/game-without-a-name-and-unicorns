
#ifndef GAME_UISCREENMANAGER_H
#define GAME_UISCREENMANAGER_H

#include "UIScreen.h"

class UIScreenManager
{

    std::list<UIScreen *> screens;

    delegate_method onResize;

  public:

    UIScreenManager();

    bool noScreens();

    UIScreen &getActiveScreen();

    void openScreen(const asset<luau::Script> &);

    void closeActiveScreen();

    ~UIScreenManager();

};


#endif
