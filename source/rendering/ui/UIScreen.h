
#ifndef GAME_UISCREEN_H
#define GAME_UISCREEN_H


#include "../../ecs/EntityEngine.h"

class UIScreen : public EntityEngine, public Screen
{
    asset<luau::Script> script;

  protected:
    void initializeLuaEnvironment() override;

  public:

    UIScreen(const asset<luau::Script> &);

    void render(double deltaTime) override;

    void onResize() override;
};


#endif
